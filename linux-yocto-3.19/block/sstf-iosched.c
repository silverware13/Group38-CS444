/*
 * elevator C-LOOK
 * Modified noop elevator
 * ----------------------
 * CS444 Spring 2018
 * Project 2: I/O Elevators
 * Zachary Thomas, Cameron Friel,
 * Jiaji Sun. 
 */

/*
 * Debug mode of 0 shows no messages.
 * Debug mode of 1 shows dispatch sector messages.
 * Debug mode of 2 shows adding request and dispatch sector messages.
 */
#define DEBUG_MODE 2
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

/*
 * Global variable that keeps track of
 * the location of the read/write head.
 */
sector_t RW_head = 0;

struct sstf_data {
	struct list_head queue;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		/* 
 		 * We can assume that the head of the dispatch queue is the
 		 * current location of the read/write head.
 		 */
		RW_head = blk_rq_pos(rq);	
		if(DEBUG_MODE) {
			/*
			 * To show that our solution is correct we want to
			 * be able to plot our sectors against time.
			 */
			printk("Dispatched from sector %llu\n", RW_head);	
		}
		return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	struct request *compare;
	struct list_head *sort_head;

	// Check if list is empty.	
	if (!list_empty(&nd->queue)) {
		/*
 		 * We check to see if the request's sector
 		 * is after the current location of the
 		 * read/write head. 
 		 */
		if(blk_rq_pos(rq) >= RW_head) {
			if(DEBUG_MODE == 2) {
				printk("Adding request from sector %llu. R/W head is in sector %llu\n", blk_rq_pos(rq), RW_head);
			}	
			/*
 			 * Since the request's sector was after 
 			 * or shares the head's sector we sort 
 			 * so it will be handled on 
 			 * the current pass.
 			 */
			list_for_each(sort_head, &nd->queue) {
				/*
 				 * THIS IS THE SORT FOR THE CURRENT PASS
 				 * We look for any sector larger
 				 * than the requested sector and 
 				 * put this sector infront of it.
 				 * If we can't find one before
 				 * we reach the head's sector
 				 * we just put ourselves in
 				 * front of the head's sector.
 				 */
				compare = list_entry(sort_head, struct request, queuelist);
				if(DEBUG_MODE == 2) {
					printk("[CURRENT] Sorting request for sector %llu. Comparing to request in sector %llu\n", blk_rq_pos(rq), blk_rq_pos(compare));
				}	
				if(blk_rq_pos(compare) >= blk_rq_pos(rq) || blk_rq_pos(compare) >= RW_head) {
					list_add_tail(&rq->queuelist, sort_head);
					if(DEBUG_MODE == 2) {
						printk("[CURRENT] Added request for sector %llu\n", blk_rq_pos(rq));
					}	
					return;
				}
			}
		} else {
			/*
 			 * Since the request's sector was before 
 			 * the head's sector we sort it so it 
 			 * will be handled on the next pass.
 			 */
			list_for_each(sort_head, &nd->queue) {
				/*
 			 	 * THIS IS THE SORT FOR THE NEXT PASS
 				 * We move past the head's sector in
 				 * the queue. Then we find a sector 
 				 * that is larger than this one and 
 				 * put this request in front of it.
 				 */
				compare = list_entry(sort_head, struct request, queuelist);
				if(DEBUG_MODE == 2) {
					printk("[NEXT] Sorting request for sector %llu. Comparing to request in sector %llu\n", blk_rq_pos(rq), blk_rq_pos(compare));
				}	
				if(blk_rq_pos(compare) >= blk_rq_pos(rq) && blk_rq_pos(compare) >= RW_head) {
					list_add_tail(&rq->queuelist, sort_head);
					if(DEBUG_MODE == 2) {
						printk("[NEXT] Added request for sector %llu\n", blk_rq_pos(rq));
					}	
					return;
				}
			}
		}
	} else {
		/*
		 * If the list is empty there is no need
		 * to sort. Just add.
		 */
		if(DEBUG_MODE == 2) {
			printk("Adding request from sector %llu. R/W head is in sector %llu. Was empty\n", blk_rq_pos(rq), RW_head);
		}	
		list_add_tail(&rq->queuelist, &nd->queue);
	}
}

static struct request *
sstf_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("Jens Axboe + group38");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
