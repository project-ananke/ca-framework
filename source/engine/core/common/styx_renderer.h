/* date = April 16th 2022 10:17 pm */

#ifndef STYX_RENDERER_H
#define STYX_RENDERER_H

// Can we do a constantly updating queue size?
#define QUEUE_MAX_JOBS 64

typedef enum StyxQueueJob StyxQueueJob;
enum StyxQueueJob
{
	QueueJob_None,
	QueueJob_Clear,
	QueueJob_Triangle,
};

typedef union StyxQueueJobData StyxQueueJobData;
union StyxQueueJobData
{
	struct ClearData 
	{
		vec3c color;
	} clear_data;
	struct TriangleData
	{
		vec3f coord1;
		vec3f coord2;
		vec3f coord3;

		vec3c col1;
		vec3c col2;
		vec3c col3;
	} triangle_data;
};

typedef struct StyxRenderQueue StyxRenderQueue;
struct StyxRenderQueue
{
	StyxQueueJob *jobs;
	StyxQueueJobData *job_data;

	u32 write_idx;
};

// There isn't anything to free if the render queue is allocated on the 
// linear allocator. 
styx_function StyxRenderQueue *styx_init_render_queue(void);
styx_function void styx_render_push(StyxRenderQueue *renderer, 
									StyxQueueJob job, StyxQueueJobData data);

#define styxctrl_push_triangle(renderer, cd1, c1, cd2, c2, cd3, c3)      \
	styx_render_push(renderer, QueueJob_Triangle,                        \
					 (StyxQueueJobData){                                 \
					 .triangle_data.coord1 = cd1,                        \
					 .triangle_data.col1 = c1,                           \
					 .triangle_data.coord2 = cd2,                        \
					 .triangle_data.col2 = c2,                           \
					 .triangle_data.coord3 = cd3,                        \
					 .triangle_data.col3 = c3})

#define styx_push_clear(renderer, col) \
	styx_render_push(renderer,  QueueJob_Clear, (StyxQueueJobData){.clear_data.color = col})
#define styx_push_triangle(renderer, cd1, cd2, cd3, col) \
	styxctrl_push_triangle(renderer, cd1, col, cd2, col, cd3, col)

#endif //STYX_RENDERER_H
