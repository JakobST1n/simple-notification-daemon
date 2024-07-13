#include "snotif.h"
#include <dbus/dbus.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct NotifsList* notifs;

void print_notification_log() {
    printf("LOG:\n");
    for (size_t i = 0; i < notifs->element_count; i++) {
        if (notifs->list[i]) {
            printf("[%zu] %s", i, strnotify_params(notifs->list[i]));
        }
    }
}

unsigned int notifid(struct NotifyParams* notify_params) {
    if (!notify_params->replaces_id || notify_params->replaces_id > notifs->element_count) {
        notifs->element_count++;
        return notifs->element_count;
    }
    return notify_params->replaces_id;
}

void reply_to_notify_method_call(DBusMessage* msg, DBusConnection* conn, struct NotifyParams* notify_params) {
    DBusMessage* reply;
    DBusMessageIter args;
    dbus_uint32_t serial = 0;

    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &args);

    unsigned int nid = notify_params->id;
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &nid)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    if (!dbus_connection_send(conn, reply, &serial)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    dbus_connection_flush(conn);

    dbus_message_unref(reply);
    printf("> [%s] notification ID %i\n", notify_params->app_name, notify_params->id);
}

void handle_notify_method_call(DBusMessage* msg, DBusConnection* conn) {
    //const char* sender = dbus_message_get_sender(msg);

    struct NotifyParams* notify_params = malloc(sizeof(struct NotifyParams));
    if (!notify_params) {
        fprintf(stderr, "Failed to allocate memory for notification parameters!\n");
        exit(1);
    }

    notify_params->time = time(NULL);
    notify_params->app_name = "";
    notify_params->app_icon = "";
    notify_params->summary = "";
    notify_params->body = "";
    notify_params->expire_timeout = -1;
    notify_params->seen = false;

    const char* tmp_str;
    DBusMessageIter args;
    dbus_message_iter_init(msg, &args);
    size_t msg_i = 0;
    do {
        switch (msg_i) {
            case 0:
                dbus_message_iter_get_basic(&args, &tmp_str);
                notify_params->app_name = strdup(tmp_str);
                break;
            case 1:
                dbus_message_iter_get_basic(&args, &notify_params->replaces_id);
                break;
            case 2:
                dbus_message_iter_get_basic(&args, &tmp_str);
                notify_params->app_icon = strdup(tmp_str);
                break;
            case 3:
                dbus_message_iter_get_basic(&args, &tmp_str);
                notify_params->summary = strdup(tmp_str);
                break;
            case 4:
                dbus_message_iter_get_basic(&args, &tmp_str);
                notify_params->body = strdup(tmp_str);
                break;
            case 7:
                dbus_message_iter_get_basic(&args, &notify_params->expire_timeout);
                break;
        }

        msg_i++;
    } while (dbus_message_iter_next(&args));
    notify_params->id = notifid(notify_params);

    printf("< %s", strnotify_params(notify_params));
    reply_to_notify_method_call(msg, conn, notify_params);
    notifs_list_set(notifs, notify_params);
}

void reply_to_get_capabilities_method_call(DBusMessage* msg, DBusConnection* conn) {
    DBusMessage* reply;
    DBusMessageIter args, array_iter;
    dbus_uint32_t serial = 0;

    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &args);
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &array_iter);

    const char* capabilities[] = {"body", "body-hyperlinks", "body-images", "body-markup"};
    for (int i = 0; i < 4; ++i) {
        if (!dbus_message_iter_append_basic(&array_iter, DBUS_TYPE_STRING, &capabilities[i])) {
            fprintf(stderr, "Out Of Memory!\n");
            exit(1);
        }
    }

    dbus_message_iter_close_container(&args, &array_iter);

    if (!dbus_connection_send(conn, reply, &serial)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    dbus_connection_flush(conn);

    dbus_message_unref(reply);
}

void reply_to_get_server_information_method_call(DBusMessage* msg, DBusConnection* conn) {
    DBusMessage* reply;
    DBusMessageIter args;
    dbus_uint32_t serial = 0;

    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &args);
    const char* name = "NotifdbNotificationServer";
    const char* vendor = "jakobst1n";
    const char* version = "1.0";
    const char* spec_version = "1.2";

    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &name) ||
            !dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &vendor) ||
            !dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &version) ||
            !dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &spec_version)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    if (!dbus_connection_send(conn, reply, &serial)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    dbus_connection_flush(conn);

    dbus_message_unref(reply);
}

void reply_to_get_notification_list(DBusMessage* msg, DBusConnection* conn) {
    DBusMessage* reply;
    DBusMessageIter args, array_iter, struct_iter;

    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &args);

    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY,
            DBUS_STRUCT_BEGIN_CHAR_AS_STRING
            DBUS_TYPE_UINT32_AS_STRING
            DBUS_TYPE_STRING_AS_STRING
            DBUS_TYPE_STRING_AS_STRING
            DBUS_TYPE_STRING_AS_STRING
            DBUS_TYPE_UINT32_AS_STRING
            DBUS_TYPE_BOOLEAN_AS_STRING
            DBUS_STRUCT_END_CHAR_AS_STRING,
            &array_iter);

    for (size_t i = 0; i < notifs->element_count; i++) {
        if (notifs->list[i]) {
            dbus_message_iter_open_container(&array_iter, DBUS_TYPE_STRUCT, NULL, &struct_iter);
            dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_UINT32, &notifs->list[i]->id);
            dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &notifs->list[i]->app_name);
            dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &notifs->list[i]->summary);
            dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &notifs->list[i]->body);
            dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_UINT32, &notifs->list[i]->time);
            unsigned int _seen = notifs->list[i]->seen; // Workaround for broken implementation
            dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_BOOLEAN, &_seen);
            dbus_message_iter_close_container(&array_iter, &struct_iter);
        }
    }
    dbus_message_iter_close_container(&args, &array_iter);

    dbus_connection_send(conn, reply, NULL);
    dbus_message_unref(reply);
}

void reply_to_get_unseen_count(DBusMessage* msg, DBusConnection* conn) {
    DBusMessage* reply;
    DBusMessageIter args;

    unsigned int unseen_count = 0;
    for (size_t i = 0; i < notifs->element_count; i++) {
        if (notifs->list[i] && !notifs->list[i]->seen)
            unseen_count++;
    }

    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &unseen_count)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    dbus_connection_send(conn, reply, NULL);
    dbus_message_unref(reply);
}

void reply_to_set_seen(DBusMessage* msg, DBusConnection* conn) {
    DBusMessage* reply;
    DBusMessageIter args;

    int notif_id = -1;
    unsigned int seen = false;

    if (!dbus_message_iter_init(msg, &args)) {
        fprintf(stderr, "Message has no arguments!\n");
        return;
    }

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_UINT32) {
        fprintf(stderr, "First argument is not an integer!\n");
        return;
    }

    dbus_message_iter_get_basic(&args, &notif_id);

    if (!dbus_message_iter_next(&args)) {
        fprintf(stderr, "Message has too few arguments!\n");
        return;
    }

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_BOOLEAN) {
        fprintf(stderr, "Second argument is not an boolean!\n");
        return;
    }
    dbus_message_iter_get_basic(&args, &seen);

    printf("< Received notif_id: %d, seen: %d\n", notif_id, seen);
    printf("< Current notification_id: %lu\n", notifs->element_count);

    if (notif_id < notifs->element_count) {
        printf("< Setting notification_log[%d]->seen to %d\n", notif_id, seen);
        notifs->list[notif_id]->seen = seen;
    } else {
        fprintf(stderr, "Invalid notif_id: %d (max: %lu)\n", notif_id, notifs->element_count - 1);
    }

    reply = dbus_message_new_method_return(msg);

    dbus_connection_send(conn, reply, NULL);
    dbus_message_unref(reply);
}

void reply_to_clear_all(DBusMessage* msg, DBusConnection* conn) {
    DBusMessage* reply;
    DBusMessageIter args;
    notifs_list_clear(notifs);
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &args);
    dbus_connection_send(conn, reply, NULL);
    dbus_message_unref(reply);
}


void init_dbus(DBusConnection** conn) {
    DBusError err;
    dbus_error_init(&err);

    *conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        fprintf(stderr, "Failed to connect to the session bus\n");
        exit(1);
    }
    printf("Connected to the DBus session bus\n");

    int ret = dbus_bus_request_name(*conn, DBUS_NOTIF_INTERFACE, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        fprintf(stderr, "Failed to request name\n");
        exit(1);
    }
    printf("DBus name \"%s\" requested successfully\n", DBUS_NOTIF_INTERFACE);

    ret = dbus_bus_request_name(*conn, DBUS_CLIENT_INTERFACE, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        fprintf(stderr, "Failed to request name\n");
        exit(1);
    }
    printf("DBus name \"%s\" requested successfully\n", DBUS_CLIENT_INTERFACE);
}

void handle_dbus_message(DBusConnection* conn, DBusMessage* msg) {
    if (NULL == msg) {
        return;
    }

    if (dbus_message_is_method_call(msg, DBUS_NOTIF_INTERFACE, "Notify")) {
        handle_notify_method_call(msg, conn);
    } else if (dbus_message_is_method_call(msg, DBUS_NOTIF_INTERFACE, "GetCapabilities")) {
        reply_to_get_capabilities_method_call(msg, conn);
    } else if (dbus_message_is_method_call(msg, DBUS_NOTIF_INTERFACE, "GetServerInformation")) {
        reply_to_get_server_information_method_call(msg, conn);
    } else if (dbus_message_is_method_call(msg, DBUS_CLIENT_INTERFACE, DBUS_METHOD_GETLIST)) {
        reply_to_get_notification_list(msg, conn);
    } else if (dbus_message_is_method_call(msg, DBUS_CLIENT_INTERFACE, DBUS_METHOD_GETUNSEENCOUNT)) {
        reply_to_get_unseen_count(msg, conn);
    } else if (dbus_message_is_method_call(msg, DBUS_CLIENT_INTERFACE, DBUS_METHOD_SETSEEN)) {
        reply_to_set_seen(msg, conn);
    } else if (dbus_message_is_method_call(msg, DBUS_CLIENT_INTERFACE, DBUS_METHOD_CLEAR)) {
        reply_to_clear_all(msg, conn);
    }

    dbus_message_unref(msg);
}

void listen_dbus() {
    DBusConnection* conn;
    DBusMessage* msg;
    init_dbus(&conn);

    while (1) {
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        if (NULL == msg) {
            usleep(10000);
            continue;
        }

        handle_dbus_message(conn, msg);
    }
}

int main(int argc, char **argv) {
    printf("Starting simple-notification-daemon\n");
    notifs_list_init(&notifs);
    listen_dbus();
    return 0;
}
