#include "snotif.h"
#include <stdio.h>
#include <stdlib.h>

char* strnotify_params(const struct NotifyParams* params) {
    char time[80] = "";
    if (params->time)
        strftime(time, 80, "%R:%S", localtime(&params->time));
    static char res[180];
    sprintf(
            res,
            "%s [%s] (timeout: %d, replaces: %u) %s\n",
            time, params->app_name, params->expire_timeout, params->replaces_id, params->summary
           );
    return res;
}

void notifs_list_init(struct NotifsList** list) {
    (*list) = malloc(sizeof(struct NotifsList));
    (*list)->list_size = 0;
    (*list)->element_count = 0;

#define INITIAL_SIZE 8
    (*list)->list = calloc(INITIAL_SIZE, sizeof(struct NotifyParams*));
    if ((*list)->list == NULL) {
        fprintf(stderr, "Failed to allocate memory for notification log!\n");
        exit(1);
    }
    (*list)->list_size = INITIAL_SIZE;
}

void notifs_list_set(struct NotifsList* list, struct NotifyParams* notif) {
    while (notif->id > list->list_size) {
        unsigned int new_size = list->list_size * 2;
        struct NotifyParams** temp = realloc(list->list, new_size * sizeof(struct NotifyParams*));
        if (!temp) {
            fprintf(stderr, "Failed to reallocate memory for notification log!\n");
            exit(1);
        }
        list->list = temp;
        for (unsigned int i = list->list_size; i < new_size; i++) {
            list->list[i] = NULL;
        }
        list->list_size = new_size;
    }

    if (notif->id > list->element_count) {
        list->element_count = notif->id;
    }
    list->list[notif->id - 1] = notif;
}

int notifs_list_set_seen(struct NotifsList* list, unsigned int id, bool read) {
    if (id > list-> list_size)
        return 1;
    list->list[id]->seen = read;
    return 0;
}

void notifs_list_free(struct NotifsList* list) {
    for (unsigned int i = 0; i < list->element_count; i++) {
        if (list->list[i]->app_name)
            free(list->list[i]->app_name);
        if (list->list[i]->app_icon)
            free(list->list[i]->app_icon);
        if (list->list[i]->summary)
            free(list->list[i]->summary);
        if (list->list[i]->body)
            free(list->list[i]->body);
        free(list->list[i]);
    }
    free(list->list);
    free(list);
}
