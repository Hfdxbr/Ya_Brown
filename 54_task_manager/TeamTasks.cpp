#include "TeamTasks.h"
#include <vector>

const TasksInfo& TeamTasks::GetPersonTasksInfo(
    const std::string& person) const {
  return tasks_by_user.at(person);
}

void TeamTasks::AddNewTask(const std::string& person) {
  if (tasks_by_user.find(person) == tasks_by_user.end()) {
    tasks_by_user.insert({person,
                          {{TaskStatus::NEW, 1},
                           {TaskStatus::IN_PROGRESS, 0},
                           {TaskStatus::TESTING, 0},
                           {TaskStatus::DONE, 0}}});
  } else
    ++tasks_by_user[person][TaskStatus::NEW];
}

std::tuple<TasksInfo, TasksInfo> TeamTasks::PerformPersonTasks(
    const std::string& person, int task_count) {
  const std::vector<TaskStatus> workflow = {
      TaskStatus::NEW, TaskStatus::IN_PROGRESS, TaskStatus::TESTING, TaskStatus::DONE};
  auto stage = workflow.begin();

  TasksInfo& person_tasks = tasks_by_user[person];
  TasksInfo changed;
  TasksInfo unchanged;

  auto done_on_prev_stage = 0;
  while (stage != --workflow.end()) {
    auto done_on_stage = task_count > person_tasks[*stage] ? person_tasks[*stage] : task_count;
    auto left_on_stage = person_tasks[*stage] - done_on_stage - done_on_prev_stage;
    task_count -= done_on_stage;
    person_tasks[*stage] -= done_on_stage;
    if (left_on_stage) unchanged[*stage] = left_on_stage;
    ++stage;
    done_on_prev_stage = done_on_stage;
    person_tasks[*stage] += done_on_stage;
    if (done_on_stage) changed[*stage] += done_on_stage;
  }

  return {changed, unchanged};
}
