#include "snotif.h"
#include "term.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_unseen_count(DBusConnection* conn) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusPendingCall* pending;

    msg = dbus_message_new_method_call(
            DBUS_CLIENT_INTERFACE,
            DBUS_CLIENT_OBJECT,
            DBUS_CLIENT_INTERFACE,
            DBUS_METHOD_GETUNSEENCOUNT
            );

    dbus_message_iter_init_append(msg, &args);

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    dbus_pending_call_block(pending);
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n"); exit(1);
    }
    dbus_pending_call_unref(pending);

    unsigned int count;
    if (!dbus_message_iter_init(msg, &args))
        fprintf(stderr, "Message has no arguments!\n");
    else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args))
        fprintf(stderr, "Argument is not uint32!\n");
    else
        dbus_message_iter_get_basic(&args, &count);
    printf("%d", count);
    fflush(stdout);

    dbus_message_unref(msg);
}

void print_arg_if_unseen(DBusConnection* conn, char* arg) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusPendingCall* pending;

    msg = dbus_message_new_method_call(
            DBUS_CLIENT_INTERFACE,
            DBUS_CLIENT_OBJECT,
            DBUS_CLIENT_INTERFACE,
            DBUS_METHOD_GETUNSEENCOUNT
            );

    dbus_message_iter_init_append(msg, &args);

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    dbus_pending_call_block(pending);
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n"); exit(1);
    }
    dbus_pending_call_unref(pending);

    unsigned int count;
    if (!dbus_message_iter_init(msg, &args))
        fprintf(stderr, "Message has no arguments!\n");
    else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args))
        fprintf(stderr, "Argument is not uint32!\n");
    else
        dbus_message_iter_get_basic(&args, &count);

    if (count > 0) {
        printf("%s", arg);
        fflush(stdout);
    }

    dbus_message_unref(msg);
}

void set_notif_seen(DBusConnection* conn, uint id, bool seen) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusPendingCall* pending;

    msg = dbus_message_new_method_call(
            DBUS_CLIENT_INTERFACE,
            DBUS_CLIENT_OBJECT,
            DBUS_CLIENT_INTERFACE,
            DBUS_METHOD_SETSEEN
            );

    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &id)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    unsigned int _seen = seen;
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &_seen)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    dbus_pending_call_block(pending);
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n"); exit(1);
    }
    dbus_pending_call_unref(pending);

    dbus_message_unref(msg);
}

void clear_all(DBusConnection* conn) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusPendingCall* pending;

    msg = dbus_message_new_method_call(
            DBUS_CLIENT_INTERFACE,
            DBUS_CLIENT_OBJECT,
            DBUS_CLIENT_INTERFACE,
            DBUS_METHOD_CLEAR
            );

    dbus_message_iter_init_append(msg, &args);

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    dbus_pending_call_block(pending);
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n"); exit(1);
    }
    dbus_pending_call_unref(pending);

    dbus_message_unref(msg);
}

void print_notification_list(DBusConnection* conn) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusPendingCall* pending;

    msg = dbus_message_new_method_call(
            DBUS_CLIENT_INTERFACE,
            DBUS_CLIENT_OBJECT,
            DBUS_CLIENT_INTERFACE,
            DBUS_METHOD_GETLIST
            );

    dbus_message_iter_init_append(msg, &args);

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    dbus_pending_call_block(pending);
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n"); exit(1);
    }
    dbus_pending_call_unref(pending);

    DBusMessageIter array_iter, struct_iter;
    if (!dbus_message_iter_init(msg, &args))
        fprintf(stderr, "Message has no arguments!\n");
    else {
        if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&args)) {
            dbus_message_iter_recurse(&args, &array_iter);

            while (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_STRUCT) {
                dbus_message_iter_recurse(&array_iter, &struct_iter);

                unsigned int id;
                const char* app_name;
                const char* summary;
                const char* body;

                dbus_message_iter_get_basic(&struct_iter, &id);
                dbus_message_iter_next(&struct_iter);
                dbus_message_iter_get_basic(&struct_iter, &app_name);
                dbus_message_iter_next(&struct_iter);
                dbus_message_iter_get_basic(&struct_iter, &summary);
                dbus_message_iter_next(&struct_iter);
                dbus_message_iter_get_basic(&struct_iter, &body);
                dbus_message_iter_next(&struct_iter);

                printf(KITAL"%10s"KNRM" ┃ "KMAG"%s"KNRM"\n", app_name, summary);

                dbus_message_iter_next(&array_iter);
            }
        }
    }

    char* response = NULL;
    if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &response, DBUS_TYPE_INVALID)) {
        printf("Received: %s\n", response);
    }
    dbus_message_unref(msg);
}

void add_to_notiflist(struct NotifsList* notifs, DBusMessageIter* struct_iter) {
    struct NotifyParams* notif = malloc(sizeof(struct NotifyParams));
    if (!notif) {
        fprintf(stderr, "Failed to allocate memory for notification parameters!\n");
        exit(1);
    }
    notif->app_icon = NULL;
    notif->time = 0;

    const char* tmp_str;
    dbus_message_iter_get_basic(struct_iter, &notif->id);

    dbus_message_iter_next(struct_iter);
    dbus_message_iter_get_basic(struct_iter, &tmp_str);
    notif->app_name = strdup(tmp_str);

    dbus_message_iter_next(struct_iter);
    dbus_message_iter_get_basic(struct_iter, &tmp_str);
    notif->summary = strdup(tmp_str);

    dbus_message_iter_next(struct_iter);
    dbus_message_iter_get_basic(struct_iter, &tmp_str);
    notif->body = strdup(tmp_str);

    dbus_message_iter_next(struct_iter);
    dbus_message_iter_get_basic(struct_iter, &notif->time);

    dbus_message_iter_next(struct_iter);
    dbus_message_iter_get_basic(struct_iter, &notif->seen);

    notifs_list_set(notifs, notif);
}

void get_notification_list(DBusConnection* conn, struct NotifsList* notifs) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusPendingCall* pending;

    msg = dbus_message_new_method_call(
            DBUS_CLIENT_INTERFACE,
            DBUS_CLIENT_OBJECT,
            DBUS_CLIENT_INTERFACE,
            DBUS_METHOD_GETLIST
            );

    dbus_message_iter_init_append(msg, &args);

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    dbus_pending_call_block(pending);
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n"); exit(1);
    }
    dbus_pending_call_unref(pending);

    DBusMessageIter array_iter, struct_iter;
    if (dbus_message_iter_init(msg, &args) && dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_ARRAY) {
        dbus_message_iter_recurse(&args, &array_iter);

        while (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_STRUCT) {
            dbus_message_iter_recurse(&array_iter, &struct_iter);
            add_to_notiflist(notifs, &struct_iter);
            dbus_message_iter_next(&array_iter);
        }
    }
    dbus_message_unref(msg);
}

void init_dbus(DBusConnection** conn) {
    DBusError err;
    dbus_error_init(&err);
    *conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error: %s\n", err.message);
        dbus_error_free(&err);
        exit(EXIT_FAILURE);
    }
}

void curses_init() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();

    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    init_pair(3, COLOR_YELLOW, COLOR_BLUE);

    init_pair(CPAIR_B, -1, -1);
    init_pair(CPAIR_B + 1, COLOR_YELLOW, -1);
    init_pair(CPAIR_B + 2, COLOR_MAGENTA, -1);
    init_pair(CPAIR_B + 3, COLOR_WHITE, -1);
    init_pair(CPAIR_B | CPAIR_SEL, -1, COLOR_CYAN);
    init_pair((CPAIR_B + 1) | CPAIR_SEL, COLOR_YELLOW, COLOR_CYAN);
    init_pair((CPAIR_B + 2) | CPAIR_SEL, COLOR_MAGENTA, COLOR_CYAN);
    init_pair((CPAIR_B + 3) | CPAIR_SEL, COLOR_WHITE, COLOR_CYAN);
}

void curses_display_menu(int c_id) {
    mvprintw(LINES - 1, 0, " F2");
    attron(COLOR_PAIR(2));
    printw("Show notification");
    attroff(COLOR_PAIR(2));

    printw(" F7");
    attron(COLOR_PAIR(2));
    printw("Toogle read      ");
    attroff(COLOR_PAIR(2));

    printw(" F8");
    attron(COLOR_PAIR(2));
    printw("Mark all read    ");
    attroff(COLOR_PAIR(2));

    printw(" F9");
    attron(COLOR_PAIR(2));
    printw("Clear            ");
    attroff(COLOR_PAIR(2));

    printw("F10");
    attron(COLOR_PAIR(2));
    printw("Quit             ");
    attroff(COLOR_PAIR(2));

    chgat(-1, A_NORMAL, 2, NULL);

    attron(COLOR_PAIR(2));
    mvprintw(LINES-1, COLS-3, "%3d", c_id);
    attroff(COLOR_PAIR(2));
}

void curses_display_notifs(struct NotifsList* notifs, int start, int selected) {
    int w_app_name = 0;
    int w = 0;

    for (size_t i = 0; i < notifs->element_count; i++) {
        if (!notifs->list[i])
            continue;

        w = strlen(notifs->list[i]->app_name);
        if (notifs->list[i]->app_name && w > w_app_name)
            w_app_name = w;
    }

    const int items = (notifs->element_count > LINES) ? LINES : notifs->element_count;
    int n_printed = 0;
    size_t ri;

    struct NotifyParams* notif;
    for (size_t i = start; i < items+start; i++) {
        ri = notifs->element_count - i - 1;
        if (!notifs->list[ri])
            continue;
        notif = notifs->list[ri];

        char time[10] = "";
        if (notif->time)
            strftime(time, 10, "%R:%S", localtime(&notif->time));

        short extra_bits = (i == selected) ? CPAIR_SEL : 0;
        attron(COLOR_PAIR(CPAIR_B | extra_bits));
        if (notif->seen) {
            attron(COLOR_PAIR((CPAIR_B + 3) | extra_bits));
            attron(A_ITALIC);
            mvprintw(i-start, 0, " %s", time);
            attroff(A_ITALIC);
            printw(" │ ");
            attron(A_ITALIC);
            printw("%*s", w_app_name, notif->app_name);
            attroff(A_ITALIC);
            printw(" │ ");
            attron(A_ITALIC);
            printw("%s", notif->summary);
            attroff(A_ITALIC);
        } else {
            attron(A_ITALIC);
            mvprintw(i-start, 0, " %s", time);
            attroff(A_ITALIC);
            printw(" │ ");
            attron(COLOR_PAIR((CPAIR_B + 2) | extra_bits));
            printw("%*s", w_app_name, notif->app_name);
            attroff(COLOR_PAIR((CPAIR_B + 2) | extra_bits));
            attron(COLOR_PAIR(CPAIR_B | extra_bits));
            printw(" │ ");
            printw("%s", notif->summary);
        }
        attroff(COLOR_PAIR(CPAIR_B | extra_bits));
        chgat(-1, A_NORMAL, CPAIR_B | extra_bits, NULL);

        n_printed++;
    }
}

void show_full_notif(int id, struct NotifsList* notifs) {
    if (id >= notifs->element_count || !notifs->list[id])
        return;

    int width = COLS;
    int height = LINES - 1;
    int start_x = 0;
    int start_y = 0;

#define MAX_WIDTH 80
    if (width > MAX_WIDTH) {
        width = MAX_WIDTH;
        start_x = ((COLS - 1) / 2) - (MAX_WIDTH / 2);
    }
#define MAX_HEIGHT 20
    if (height > MAX_HEIGHT) {
        height = MAX_HEIGHT;
        start_y = ((LINES - 1) / 2) - (MAX_HEIGHT / 2);
    }

    WINDOW *win = newwin(height, width, start_y, start_x);
    WINDOW *win_content = newwin(height-2, width-2, start_y+1, start_x+1);
    while (1) {
        box(win, 0, 0);

        wattron(win, COLOR_PAIR(2));
        mvwprintw(win, 0, 1, "%s", notifs->list[id]->app_name);
        mvwprintw(win, height-1, 1, "q: close");
        wattroff(win, COLOR_PAIR(2));

        mvwprintw(win_content, 1, 1, "%s", notifs->list[id]->summary);

        for (size_t i = 0; i < width - 2; i++) {
            mvwprintw(win_content, 2, i, "─");
        }

        mvwprintw(win_content, 3, 1, "%s", notifs->list[id]->body);

        wrefresh(win);
        wrefresh(win_content);
        refresh();

        int ch = getch();
        if (ch == 'q' || ch == KEY_F(10))
            break;
    }

    werase(win_content);
    wrefresh(win_content);
    delwin(win_content);
    werase(win);
    wrefresh(win);
    delwin(win);
}

void curses_handle_input(DBusConnection* conn, int ch, int* start, int* selected, struct NotifsList* notifs) {
    unsigned int id = notifs->element_count - (*selected) - 1;
    WINDOW* win;
    WINDOW* progress;
    switch (ch) {
        case KEY_DOWN:
        case 'j':
            if ((*selected) < notifs->element_count-1) {
                (*selected)++;
                if ((*selected) + (*start) > LINES-2)  {
                    (*start)++;
                    clear();
                }
            }
            break;
        case KEY_UP:
        case 'k':
            if ((*selected) > 0) {
                (*selected)--;
                if ((*selected) < (*start)) {
                    (*start)--;
                    clear();
                }
            }
            break;
        case KEY_ENTER:
        case '\n':
        case '\r':
        case KEY_F(2):
            show_full_notif(id, notifs);
            break;
        case KEY_F(7):
            set_notif_seen(conn, id, !(notifs->list[id]->seen));
            break;
        case KEY_F(8):
            win = newwin(8, 30, (LINES/2)-5, (COLS/2)-15);
            box(win, 0, 0);
            wattron(win, COLOR_PAIR(2));
            mvwprintw(win, 0, 1, "%s", "Working");
            wattroff(win, COLOR_PAIR(2));
            mvwprintw(win, 2, 5, "%s", "Marking all as read");

            progress = newwin(3, 28, (LINES/2)-1, (COLS/2)-14);
            box(progress, 0, 0);
            mvwprintw(progress, 0, 1, "%s", "Progress");

            wrefresh(win);
            wrefresh(progress);
            refresh();

            unsigned int n = notifs->element_count / 25;
            for (size_t i = 0; i < notifs->element_count; i++) {
                if (notifs->list[i] && !notifs->list[i]->seen) {
                    set_notif_seen(conn, i, true);
                }
                if (i > 0) {
                    for (unsigned int o = 0; o < i/n; o++) {
                        wattron(progress, A_REVERSE);
                        mvwprintw(progress, 1, o + 1, " ");
                        wattroff(progress, A_REVERSE);
                    }
                    wrefresh(progress);
                    refresh();
                }
            }
            werase(progress);
            wrefresh(progress);
            delwin(progress);
            werase(win);
            wrefresh(win);
            delwin(win);
            break;
        case KEY_F(9):
            clear_all(conn);
            break;
    }
}

void update_notif_list(DBusConnection* conn, struct NotifsList** notifs, int* unread_count) {
    struct NotifsList* new_notifs = NULL;
    notifs_list_init(&new_notifs);
    get_notification_list(conn, new_notifs);

    notifs_list_free(*notifs);
    (*notifs) = new_notifs;

    (*unread_count) = 0;
    for (unsigned int i = 0; i < (*notifs)->element_count; i++) {
        if ((*notifs)->list[i] && !(*notifs)->list[i]->seen)
            (*unread_count)++;
    }
}

void run_tui_client(DBusConnection* conn) {
    int ch;
    int start = 0;
    int selected = 0;
    int unread_count = 0;

    setlocale(LC_ALL, "");
    curses_init();

    int last_lines = LINES;
    int last_cols = COLS;
    int last_notifs = 0;

    struct NotifsList* notifs = NULL;
    notifs_list_init(&notifs);

    while (1) {
        timeout(500);
        ch = getch();

        if (ch == KEY_F(10) || ch == 'q') {
            break;
        }

        curses_handle_input(conn, ch, &start, &selected, notifs);
        update_notif_list(conn, &notifs, &unread_count);

        bool major_change = 0;
        if (LINES != last_lines) {
            last_lines = LINES;
            major_change = true;
        }
        if (COLS != last_cols) {
            last_cols = COLS;
            major_change = true;
        }
        if (notifs->element_count != last_notifs) {
            last_notifs = notifs->element_count;
            major_change = true;
        }
        if (major_change)
            clear();

        curses_display_notifs(notifs, start, selected);
        curses_display_menu(unread_count);
        refresh();
    }

    endwin();
    notifs_list_free(notifs);
}

void usage(char **argv) {
    printf("Usage: %s <command>\n", argv[0]);
    printf("Commands:\n");
    printf("  %s unread <output if unread notifs>\n", argv[0]);
    printf("  %s unread-count\n", argv[0]);
    printf("  %s ls\n", argv[0]);
}

int main(int argc, char **argv) {
    DBusConnection* conn;
    init_dbus(&conn);

    if (argc == 1) {
        run_tui_client(conn);
        exit(0);
    }

#define UNREAD "unread"
#define UNREAD_COUNT "unread-count"
#define LS "ls"

    if (!strncmp(argv[1], UNREAD_COUNT, strlen(UNREAD_COUNT)))
        print_unseen_count(conn);
    else if (!strncmp(argv[1], LS, strlen(LS)))
        print_notification_list(conn);
    else if (!strncmp(argv[1], UNREAD, strlen(UNREAD))) {
        if (argc != 3) {
            usage(argv);
            exit(1);
        }
        char* arg = argv[2];
        print_arg_if_unseen(conn, arg);
    } else {
        usage(argv);
        exit(1);
    }

    dbus_connection_unref(conn);
    return 0;
}
