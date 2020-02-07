#include "TeamTasks.h"

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
  TasksInfo& person_tasks = tasks_by_user[person];
  TasksInfo changed;
  TasksInfo unchanged;

  while (task_count--) {
    if (person_tasks[TaskStatus::NEW]) {
      ++changed[TaskStatus::IN_PROGRESS];
      --person_tasks[TaskStatus::NEW];
    } else if (person_tasks[TaskStatus::IN_PROGRESS]) {
      ++changed[TaskStatus::TESTING];
      --person_tasks[TaskStatus::IN_PROGRESS];
    } else if (person_tasks[TaskStatus::TESTING]) {
      ++changed[TaskStatus::DONE];
      --person_tasks[TaskStatus::TESTING];
    } else
      break;
  }

  for(const auto&[stage, num] : person_tasks)
    if(num > 0 && stage != TaskStatus::DONE)
      unchanged[stage] = num;

  for(const auto&[stage, num] : changed)
      person_tasks[stage] += num;

  return {changed, unchanged};
}
