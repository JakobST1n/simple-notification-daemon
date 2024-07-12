#ifndef SNOTIF_H
#define SNOTIF_H

#include <dbus/dbus.h>
#include <stdbool.h>
#include <time.h>

// Constansts for the DBus interactions
#define  DBUS_NOTIF_INTERFACE        "org.freedesktop.Notifications"
#define  DBUS_CLIENT_INTERFACE       "snotifd.Notifications"
#define  DBUS_CLIENT_OBJECT          "/snotifd/Notifications"
#define  DBUS_METHOD_GETLIST         "GetNotifications"
#define  DBUS_METHOD_GETUNSEENCOUNT  "GetUnseenCount"
#define  DBUS_METHOD_SETSEEN         "SetSeen"

// Curses related constants
#define CPAIR_SEL 132
#define CPAIR_B 4

// Extended struct from the specification at:
// https://specifications.freedesktop.org/notification-spec/latest/ar01s09.html
struct NotifyParams {
    unsigned int id;
    char* app_name;
    dbus_uint32_t replaces_id;
    char* app_icon;
    char* summary;
    char* body;
    dbus_int32_t expire_timeout;
    time_t time;
    bool seen;
};

// Convenient struct for keeping a list of notifications
struct NotifsList {
    struct NotifyParams** list;
    size_t element_count;
    size_t list_size;
};

// Convenient methods for working with a list of notifications
char* strnotify_params(const struct NotifyParams* params);
void notifs_list_init(struct NotifsList** list);
void notifs_list_set(struct NotifsList* list, struct NotifyParams* notif);
void notifs_list_free(struct NotifsList* list);
int notifs_list_set_seen(struct NotifsList* list, unsigned int id, bool read);

#endif
