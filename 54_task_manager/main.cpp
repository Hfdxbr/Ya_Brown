#include <iostream>

#include "TeamTasks.h"

using namespace std;

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
  cout << tasks_info[TaskStatus::NEW] << " new tasks"
       << ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"
       << ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested"
       << ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
  TeamTasks tasks;
  for (int i = 0; i < 5; ++i) {
    tasks.AddNewTask("Alice");
  }
  cout << "Alice's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

  TasksInfo updated_tasks, untouched_tasks;

  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Alice", 5);
  cout << "Updated Alice's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Alice's tasks: ";
  PrintTasksInfo(untouched_tasks);

  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Alice", 5);
  cout << "Updated Alice's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Alice's tasks: ";
  PrintTasksInfo(untouched_tasks);

  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Alice", 1);
  cout << "Updated Alice's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Alice's tasks: ";
  PrintTasksInfo(untouched_tasks);

  for (int i = 0; i < 5; ++i) {
    tasks.AddNewTask("Alice");
  }

  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Alice", 2);
  cout << "Updated Alice's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Alice's tasks: ";
  PrintTasksInfo(untouched_tasks);

  cout << "Alice's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));

  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Alice", 4);
  cout << "Updated Alice's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Alice's tasks: ";
  PrintTasksInfo(untouched_tasks);

  cout << "Alice's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Alice"));
  return 0;
}