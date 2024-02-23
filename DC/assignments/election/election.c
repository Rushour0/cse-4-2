#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct ProcessNode
{
    int id;
    bool alive;
    bool coordinator;
    struct ProcessNode *next;
};

struct ProcessNode *createProcessNode(int id)
{
    struct ProcessNode *new_process = (struct ProcessNode *)malloc(sizeof(struct ProcessNode));
    if (new_process == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_process->id = id;
    new_process->alive = 0;
    new_process->coordinator = 0;
    new_process->next = NULL;
    return new_process;
}

struct ProcessNode *generateProcessNodeList(int num_processes)
{
    if (num_processes <= 0)
    {
        printf("Invalid number of processes.\n");
        exit(1);
    }

    struct ProcessNode *head = NULL;
    struct ProcessNode *current = NULL;

    for (int i = 0; i < num_processes; i++)
    {
        struct ProcessNode *new_process = createProcessNode(i + 1);

        if (head == NULL)
        {
            head = new_process;
            current = new_process;
        }
        else
        {
            current->next = new_process;
            current = new_process;
        }
    }

    return head;
}

void startProcesses(struct ProcessNode *head)
{
    struct ProcessNode *current = head;
    while (current != NULL)
    {
        current->alive = true;
        current = current->next;
    }
}

void assignCoordinator(struct ProcessNode *head)
{
    struct ProcessNode *max_process = head;
    struct ProcessNode *current = head->next;
    while (current != NULL)
    {
        if (current->id > max_process->id)
        {
            max_process = current;
        }
        current = current->next;
    }
    max_process->coordinator = true;
}

void displayTable(struct ProcessNode *head)
{
    printf("%-15s%-15s%-15s\n", "Process Name", "Alive/Dead", "Role");
    struct ProcessNode *current = head;
    while (current != NULL)
    {
        printf("%-15d%-15s%-15s\n", current->id, (current->alive ? "Alive" : "Dead"), (current->coordinator ? "Coordinator" : "-"));
        current = current->next;
    }
}

void failProcess(struct ProcessNode *head)
{
    int process_id;
    printf("Enter the ID of the process to fail: ");
    scanf("%d", &process_id);

    struct ProcessNode *current = head;
    while (current != NULL)
    {
        if (current->id == process_id)
        {
            current->alive = false;
            current->coordinator = false;
            break;
        }
        current = current->next;
    }
}

void startRingElection(struct ProcessNode *head)
{
    int initiator_id;
    printf("Enter the ID of the process initiating the election: ");
    scanf("%d", &initiator_id);

    struct ProcessNode *current = head;
    struct ProcessNode *p1 = head;
    struct ProcessNode *p2 = head;
    struct ProcessNode *end = head;
    while (current != NULL)
    {
        if (current->id == initiator_id)
        {
            p1 = current;
            p2 = current;
            printf("Election initiated by Process %d\n", initiator_id);
            int new_coordinator_id = initiator_id;
            while (true)
            {
                while (p1 == p2 || !(p2->alive))
                {
                    if (p2->next == NULL)
                    {
                        p2->next = head;
                        end = p2;
                    }
                    p2 = p2->next;
                }

                printf("Process %d sends Election message to Process %d\n", p1->id, p2->id);
                if (p2->id == new_coordinator_id)
                {
                    printf("Process %d equal to current max value\n", p2->id);
                    p2->coordinator = true;
                    break;
                }
                if (new_coordinator_id < p2->id)
                {
                    new_coordinator_id = p2->id;
                }

                p1 = p2;
            }

            printf("New Coordinator: Process %d\n", new_coordinator_id);
            printf("New coordinator %d sends coordinator message to rest of the nodes\n", new_coordinator_id);
            break;
        }
        current = current->next;
    }
    end->next = NULL;
}

void startBullyElection(struct ProcessNode *head)
{
    int initiator_id;
    printf("Enter the ID of the process initiating the election: ");
    scanf("%d", &initiator_id);

    struct ProcessNode *current = head;
    struct ProcessNode *start = head;
    while (current != NULL)
    {
        if (current->id == initiator_id)
        {
            start = current;
            printf("Election initiated by Process %d\n", initiator_id);
            int new_coordinator_id = initiator_id;
            while (start != NULL)
            {
                if (start->alive)
                {
                    struct ProcessNode *temp = start->next;
                    while (temp != NULL)
                    {
                        printf("Node %d sends \"ELECTION\" message to Node %d\n", start->id, temp->id);
                        temp = temp->next;
                    }
                    temp = start->next;
                    while (temp != NULL)
                    {
                        if (temp->alive)
                        {
                            printf("Node %d sends \"OK\" message to Node %d\n", temp->id, start->id);
                            if (new_coordinator_id < temp->id)
                            {
                                new_coordinator_id = temp->id;
                            }
                        }
                        temp = temp->next;
                    }
                }
                start = start->next;
            }
            current = head;
            while (current->id != new_coordinator_id)
            {
                current = current->next;
            }

            current->coordinator = true;
            printf("New Coordinator: Process Node %d\n", new_coordinator_id);
            printf("Coordinator Node %d sends \"COORDINATOR\" message to rest of the nodes\n", new_coordinator_id);
            break;
        }
        current = current->next;
    }
}

int main()
{
    int num_processes, choice;
    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);
    struct ProcessNode *process_list = generateProcessNodeList(num_processes);
    startProcesses(process_list);
    assignCoordinator(process_list);
    displayTable(process_list);

    do
    {
        printf("\nMenu:\n");
        printf("1. Fail a process\n");
        printf("2. Call bully election\n");
        printf("3. Call ring election\n");
        printf("4. Show the status\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            failProcess(process_list);
            displayTable(process_list);
            break;

        case 2:
            startBullyElection(process_list);
            break;

        case 3:
            startRingElection(process_list);
            break;

        case 4:
            displayTable(process_list);
            break;

        case 5:
            printf("Exiting program.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 5);

    return 0;
}
