#include "Algorithm.h"

using namespace grpc;

TaskPathPlanReply Algorithm::task_assign_path_plan(TaskPathPlanRequest& request)
{
	TaskPathPlanReply reply;
	ClientContext context;

	Status status = _stub->taskPathPlan(&context, request, &reply);

	if (!status.ok()) {
		std::cout << status.error_code() << ": " << status.error_message() << std::endl;
		std::cout << "recieve error" << std::endl;
		return TaskPathPlanReply();
	}

	return reply;
}