/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of duihome.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "ut_homeapplication.h"
#include <QtGui>
#include <QDBusPendingReply>
#include <QDBusAbstractInterface>
#include <QDBusMessage>
#include <QDBusConnection>
#include <DuiScene>
#include <DuiSceneManager>
#include <DuiLocale>
#include "homeapplication.h"
#include "notificationmanager_stub.h"
#include "homenotificationsink.h"
#include "duicompositornotificationsink.h"
#include "duifeedbacknotificationsink.h"
#include "x11wrapper.h"
#include "x11helper.h"
#include "eventtypestore_stub.h"
#include "mainwindow_stub.h"
#include "duiwindow_stub.h"
#include "duiapplication_stub.h"
#include "testcontextitem.h"
#include "contextframeworkcontext.h"
#include <signal.h>

#define ATOM_TYPE 0x00010000
#define ATOM_TYPE_NORMAL 0x00010001
#define ATOM_TYPE_DESKTOP 0x00010002
#define ATOM_TYPE_NOTIFICATION 0x00010003
#define ATOM_TYPE_DOCK 0x00010004
#define ATOM_TYPE_CALL 0x00010005
#define ATOM_TYPE_SOMETHING_ELSE 0x00010006
#define ATOM_CLIENT_LIST 0x00020000
#define ATOM_CLOSE_WINDOW 0x00030000
#define ATOM_WM_STATE_SKIP_TASKBAR 0x00040001
#define WINDOW_ATTRIBUTE_TEST_WINDOWS 16
#define WINDOW_TYPE_TEST_WINDOWS 9
#define NET_WM_STATE_WINDOWS 1
#define NUMBER_OF_WINDOWS (WINDOW_ATTRIBUTE_TEST_WINDOWS + WINDOW_TYPE_TEST_WINDOWS + NET_WM_STATE_WINDOWS)

// QCoreApplication stubs to avoid crashing in processEvents()
QStringList QCoreApplication::arguments()
{
    return QStringList();
}

void DuiLocale::setDefault(const DuiLocale &)
{
}

// X11 Stubs
Atom X11Wrapper::XInternAtom(Display *, const char *atom_name, Bool)
{
    if (strcmp(atom_name, "_NET_WM_WINDOW_TYPE") == 0) {
        return ATOM_TYPE;
    } else if (strcmp(atom_name, "_NET_WM_WINDOW_TYPE_NORMAL") == 0) {
        return ATOM_TYPE_NORMAL;
    } else if (strcmp(atom_name, "_NET_WM_WINDOW_TYPE_DESKTOP") == 0) {
        return ATOM_TYPE_DESKTOP;
    } else if (strcmp(atom_name, "_NET_WM_WINDOW_TYPE_NOTIFICATION") == 0) {
        return ATOM_TYPE_NOTIFICATION;
    } else if (strcmp(atom_name, "_NET_WM_WINDOW_TYPE_DOCK") == 0) {
        return ATOM_TYPE_DOCK;
    } else if (strcmp(atom_name, "_NET_WM_WINDOW_TYPE_CALL") == 0) {
        return ATOM_TYPE_CALL;
    } else if (strcmp(atom_name, "_NET_CLIENT_LIST") == 0) {
        return ATOM_CLIENT_LIST;
    } else if (strcmp(atom_name, "_NET_CLOSE_WINDOW") == 0) {
        return ATOM_CLOSE_WINDOW;
    } else if (strcmp(atom_name, "_NET_WM_STATE_SKIP_TASKBAR") == 0) {
        return ATOM_WM_STATE_SKIP_TASKBAR;
    }

    return 0;
}

int X11Wrapper::XSelectInput(Display *, Window window, long mask)
{
    if ((mask & VisibilityChangeMask) != 0) {
        Ut_HomeApplication::visibilityNotifyWindows.append(window);
    }
    return 0;
}

Status X11Wrapper::XGetWindowAttributes(Display *, Window w, XWindowAttributes *window_attributes_return)
{
    memset(window_attributes_return, 0, sizeof(XWindowAttributes));
    window_attributes_return->map_state = IsUnmapped;

    switch (w) {
    case 0:
        return 0;
    case 1:
        break;
    case 2:
        window_attributes_return->width = 1;
        break;
    case 3:
        window_attributes_return->width = 1;
        window_attributes_return->height = 1;
        break;
    case 4:
        window_attributes_return->height = 1;
        break;
    case 5:
        window_attributes_return->height = 1;
        window_attributes_return->c_class = InputOutput;
        break;
    case 6:
        window_attributes_return->c_class = InputOutput;
        break;
    case 7:
        window_attributes_return->width = 1;
        window_attributes_return->c_class = InputOutput;
        break;
    case 8:
        window_attributes_return->width = 1;
        window_attributes_return->height = 1;
        window_attributes_return->c_class = InputOutput;
        break;
    case 9:
        window_attributes_return->map_state = IsViewable;
        break;
    case 10:
        window_attributes_return->width = 1;
        window_attributes_return->map_state = IsViewable;
        break;
    case 11:
        window_attributes_return->width = 1;
        window_attributes_return->height = 1;
        window_attributes_return->map_state = IsViewable;
        break;
    case 12:
        window_attributes_return->height = 1;
        window_attributes_return->map_state = IsViewable;
        break;
    case 13:
        window_attributes_return->height = 1;
        window_attributes_return->c_class = InputOutput;
        window_attributes_return->map_state = IsViewable;
        break;
    case 14:
        window_attributes_return->c_class = InputOutput;
        window_attributes_return->map_state = IsViewable;
        break;
    case 15:
        window_attributes_return->width = 1;
        window_attributes_return->c_class = InputOutput;
        window_attributes_return->map_state = IsViewable;
        break;
    default:
        window_attributes_return->width = 100 + w;
        window_attributes_return->height = 200 + w;
        window_attributes_return->c_class = InputOutput;
        window_attributes_return->map_state = IsViewable;
        break;
    }
    return 1;
}

int X11Wrapper::XGetWindowProperty(Display *dpy, Window w, Atom property, long long_offset, long long_length, Bool del, Atom req_type, Atom *actual_type_return, int *actual_format_return, unsigned long *nitems_return, unsigned long *bytes_after_return, unsigned char **prop_return)
{
    Q_UNUSED(long_offset);
    Q_UNUSED(long_length);
    Q_UNUSED(del);
    Q_UNUSED(req_type);
    Q_UNUSED(actual_type_return);
    Q_UNUSED(actual_format_return);
    Q_UNUSED(bytes_after_return);

    if (property == ATOM_CLIENT_LIST) {
        if (w != DefaultRootWindow(dpy)) {
            return BadWindow;
        } else {
            *nitems_return = Ut_HomeApplication::clientListNumberOfWindows;
            *prop_return = new unsigned char[Ut_HomeApplication::clientListNumberOfWindows * sizeof(Window)];

            Window *windows = (Window *) * prop_return;
            for (int i = 0; i < Ut_HomeApplication::clientListNumberOfWindows; i++)
                windows[i] = i + 1;
            return Success;
        }
    } else if (property == ATOM_TYPE) {
        Atom *atom;

        switch (w) {
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 0):
            *nitems_return = 0;
            *prop_return = new unsigned char[0];
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 1):
            *nitems_return = 1;
            *prop_return = new unsigned char[sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_DESKTOP;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 2):
            *nitems_return = 1;
            *prop_return = new unsigned char[sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_NOTIFICATION;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 3):
            *nitems_return = 1;
            *prop_return = new unsigned char[sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_DOCK;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 4):
            *nitems_return = 1;
            *prop_return = new unsigned char[sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_NORMAL;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 5):
            *nitems_return = 2;
            *prop_return = new unsigned char[2 * sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_NORMAL;
            atom[1] = ATOM_TYPE_DESKTOP;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 6):
            *nitems_return = 2;
            *prop_return = new unsigned char[2 * sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_NORMAL;
            atom[1] = ATOM_TYPE_NOTIFICATION;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 7):
            *nitems_return = 3;
            *prop_return = new unsigned char[3 * sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_NORMAL;
            atom[1] = ATOM_TYPE_DESKTOP;
            atom[2] = ATOM_TYPE_NOTIFICATION;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 8):
            *nitems_return = 2;
            *prop_return = new unsigned char[2 * sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_NORMAL;
            atom[1] = ATOM_TYPE_SOMETHING_ELSE;
            break;
        case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 9):
            *nitems_return = 1;
            *prop_return = new unsigned char[sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_CALL;
            break;
        default:
            *nitems_return = 1;
            *prop_return = new unsigned char[1 * sizeof(Atom)];
            atom = (Atom *) * prop_return;
            atom[0] = ATOM_TYPE_NORMAL;
            break;
        }
        return Success;
    } else
        return BadAtom;
}

int X11Wrapper::XFree(void *data)
{
    if (data != NULL) {
        delete [](unsigned char *)data;
    }
    return 0;
}

Status X11Wrapper::XGetWMName(Display *, Window w, XTextProperty *text_prop_return)
{
    switch (w) {
    case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 0):
        text_prop_return->value = new unsigned char[15];
        strcpy((char *)text_prop_return->value, "plain_x_window");
        return 1;
    case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 4):
        text_prop_return->value = new unsigned char[5];
        strcpy((char *)text_prop_return->value, "Test");
        return 1;
    case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 8):
        text_prop_return->value = new unsigned char[5];
        strcpy((char *)text_prop_return->value, "Tzzt");
        return 1;
    case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 9):
        text_prop_return->value = new unsigned char[8];
        strcpy((char *)text_prop_return->value, "Call_ui");
        return 1;
    default:
        return 0;
    }
}

XWMHints *X11Wrapper::XGetWMHints(Display *, Window w)
{
    XWMHints *wmhints = NULL;
    switch (w) {
    case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 4):
        wmhints = (XWMHints *)new unsigned char[sizeof(XWMHints)];
        wmhints->icon_pixmap = 202;
        break;
    case(WINDOW_ATTRIBUTE_TEST_WINDOWS + 8):
        wmhints = (XWMHints *)new unsigned char[sizeof(XWMHints)];
        wmhints->icon_pixmap = 303;
        break;
    default:
        break;
    }

    return wmhints;
}

QVector<Atom> X11Helper::getNetWmState(Display *display, Window window)
{
    Q_UNUSED(display);

    QVector<Atom> returnValue;

    if (window == WINDOW_ATTRIBUTE_TEST_WINDOWS + WINDOW_TYPE_TEST_WINDOWS + 1) {
        returnValue.append(ATOM_WM_STATE_SKIP_TASKBAR);
    }

    return returnValue;
}

void XSetWMProperties(Display *, Window, XTextProperty *, XTextProperty *, char **, int, XSizeHints *, XWMHints *, XClassHint *)
{
}

// DuiServiceFwBaseIf stubs (used by StatusArea through service framework)
QString DuiServiceFwBaseIf::resolveServiceName(const QString &ifName, const QString &preferredService)
{
    Q_UNUSED(preferredService);
    Ut_HomeApplication::serviceInterfaces.append(ifName);
    return "";
}

bool DuiServiceFwBaseIf::isValid() const
{
    if (Ut_HomeApplication::validInterfaces.contains(interface)) {
        return true;
    }
    return false;
}

// QDBusAbstractInterface stubs (used by HomeApplication through service framework)
QDBusPendingCall QDBusAbstractInterface::asyncCallWithArgumentList(const QString &method, const QList<QVariant> & args)
{
    Ut_HomeApplication::asyncCallMethods.append(method);
    Ut_HomeApplication::asyncCallArguments.append(args);

    QDBusPendingReply<bool> reply;
    return reply;
}

// QDBusPendingReplyData stubs (used by StatusArea through service framework)
QVariant QDBusPendingReplyData::argumentAt(int index) const
{
    Q_UNUSED(index);
    return QVariant();
}

void QDBusPendingReplyData::setMetaTypes(int count, const int *metaTypes)
{
    Q_UNUSED(count);
    Q_UNUSED(metaTypes);
}

maemosec::storage::~storage()
{
}

Notification::~Notification()
{
}

NotificationGroup::~NotificationGroup()
{
}

NotificationParameters::NotificationParameters()
{
}

NotificationParameters::~NotificationParameters()
{
}

// NotificationSink stubs (used by HomeNotificationSink)
void NotificationSink::addGroup(uint, const NotificationParameters &)
{
}

void NotificationSink::removeGroup(uint)
{
}

NotificationSink::NotificationSink()
{
}

bool NotificationSink::applicationEventsEnabled()
{
    return true;
}

bool NotificationSink::canAddNotification(const Notification &/*notification*/)
{
    return true;
}

QHash<NotificationSink *, bool> sinkSetEnabled;
void NotificationSink::setApplicationEventsEnabled(bool enabled)
{
    sinkSetEnabled[this] = enabled;
}


// WidgetNotificationSink stubs (used by HomeNotificationSink)
void WidgetNotificationSink::infoBannerClicked()
{
}

// HomeNotificationSink stubs (used by NotificationArea)
HomeNotificationSink *homeNotificationSink = 0;
HomeNotificationSink::HomeNotificationSink()
{
    homeNotificationSink = this;
}

HomeNotificationSink::~HomeNotificationSink()
{
}

void HomeNotificationSink::addNotification(const Notification &)
{
}

void HomeNotificationSink::removeNotification(uint)
{
}

void HomeNotificationSink::notificationAnimationDone()
{
}

void HomeNotificationSink::timeout()
{
}

void HomeNotificationSink::setNotificationAreaVisible(bool)
{
}

bool HomeNotificationSink::canAddNotification(const Notification &/*notification*/)
{
    return true;
}


// DuiCompositorNotificationSink stubs (used by NotificationArea)
DuiCompositorNotificationSink *duiCompositorNotificationSink = 0;
DuiCompositorNotificationSink::DuiCompositorNotificationSink()
{
    duiCompositorNotificationSink = this;
}

DuiCompositorNotificationSink::~DuiCompositorNotificationSink()
{
}

void DuiCompositorNotificationSink::addNotification(const Notification &)
{
}

void DuiCompositorNotificationSink::removeNotification(uint)
{
}

void DuiCompositorNotificationSink::timeout()
{
}

void DuiCompositorNotificationSink::rotateInfoBanners(const Dui::Orientation &)
{
}

// DuiFeedbackNotificationSink stubs (used by NotificationArea)
DuiFeedbackNotificationSink *duiFeedbackNotificationSink = 0;
DuiFeedbackNotificationSink::DuiFeedbackNotificationSink()
{
    duiFeedbackNotificationSink = this;
}

DuiFeedbackNotificationSink::~DuiFeedbackNotificationSink()
{
}

void DuiFeedbackNotificationSink::addNotification(const Notification &)
{
}

void DuiFeedbackNotificationSink::removeNotification(uint)
{
}

TestContextItem *testContextItem;
ContextItem *ContextFrameworkContext::createContextItem(const QString& /*key*/)
{
    testContextItem = new TestContextItem;
    return testContextItem;
}
// stubs needed for the d-bus signal and process stopping

static pid_t signalPid = 0;
static int signalValue = 0;
extern "C" int kill(pid_t pid, int sig)
{
    signalPid = pid;
    signalValue = sig;
    return 0;
}

static const QString HOME_READY_SIGNAL_PATH = "/com/nokia/duihome";
static const QString HOME_READY_SIGNAL_INTERFACE =
    "com.nokia.duihome.readyNotifier";
static const QString HOME_READY_SIGNAL_NAME = "ready";

static QString dbusMessagePath;
static QString dbusMessageInterface;
static QDBusMessage::MessageType dbusMessageType = QDBusMessage::InvalidMessage;
static QString dbusMessageMember;

bool QDBusConnection::send(const QDBusMessage &message) const
{
    dbusMessagePath = message.path();
    dbusMessageInterface = message.interface();
    dbusMessageType = message.type();
    dbusMessageMember = message.member();
    return true;
}

ContextFrameworkItem::ContextFrameworkItem(const QString &key)
    : property(key)
{
    connect(&property, SIGNAL(valueChanged()), this, SIGNAL(contentsChanged()));
}

QVariant ContextFrameworkItem::value() const
{
    return property.value();
}

// Test class
class TestHomeApplication : public HomeApplication
{
public:
    TestHomeApplication(int &argc, char **argv);

    virtual ~TestHomeApplication();

    bool testX11EventFilter(XEvent *event);

};

TestHomeApplication::TestHomeApplication(int &argc, char **argv) :
    HomeApplication(argc, argv)
{
}

TestHomeApplication::~TestHomeApplication()
{
}

bool TestHomeApplication::testX11EventFilter(XEvent *event)
{
    return HomeApplication::x11EventFilter(event);
}

QList<Window> Ut_HomeApplication::visibilityNotifyWindows;
QList<QString> Ut_HomeApplication::validInterfaces;
QList<QString> Ut_HomeApplication::serviceInterfaces;
QList<QString> Ut_HomeApplication::asyncCallMethods;
QList< QList<QVariant> > Ut_HomeApplication::asyncCallArguments;
int Ut_HomeApplication::clientListNumberOfWindows;

void Ut_HomeApplication::initTestCase()
{
    qRegisterMetaType<Window>("Window");
    gDuiApplicationStub->stubSetReturnValue("x11EventFilter", false);
}

void Ut_HomeApplication::cleanupTestCase()
{
}

void Ut_HomeApplication::init()
{
    validInterfaces.clear();
    serviceInterfaces.clear();
    asyncCallMethods.clear();
    asyncCallArguments.clear();
    clientListNumberOfWindows = NUMBER_OF_WINDOWS;

    static char *args[] = {(char *) "./ut_homeapplication"};
    static int argc = sizeof(args) / sizeof(char *);
    m_subject = new TestHomeApplication(argc, args);
    visibilityNotifyWindows.clear();
}

void Ut_HomeApplication::cleanup()
{
    delete m_subject;
}

static void resetDbusAndSignalExpectedValues()
{
    signalPid = 0;
    signalValue = 0;
    dbusMessagePath = "";
    dbusMessageInterface = "";
    dbusMessageType = QDBusMessage::InvalidMessage;
    dbusMessageMember = "";
}

static void compareDbusValues()
{
    QCOMPARE(dbusMessagePath, HOME_READY_SIGNAL_PATH);
    QCOMPARE(dbusMessageInterface, HOME_READY_SIGNAL_INTERFACE);
    QCOMPARE(dbusMessageType, QDBusMessage::SignalMessage);
    QCOMPARE(dbusMessageMember, HOME_READY_SIGNAL_NAME);
}

void Ut_HomeApplication::testUpstartStartup()
{
    resetDbusAndSignalExpectedValues();
    delete m_subject;
    char *args[] = {(char *) "./ut_homeapplication",
                    (char *) "--upstart"
                   };
    int argc = sizeof(args) / sizeof(char *);
    m_subject = new TestHomeApplication(argc, args);
    QCoreApplication::processEvents();
    // in upstart mode, dbus-signal is sent and process is stopped
    QCOMPARE(signalPid, getpid());
    QCOMPARE(signalValue, SIGSTOP);
    compareDbusValues();
}

void Ut_HomeApplication::testNonUpstartStartup()
{
    resetDbusAndSignalExpectedValues();
    delete m_subject;
    // check that any option containing 'u' is not interpreted
    // as the upstart param. Practical example is -output-level.
    char *args[] = {(char *) "./ut_homeapplication",
                    (char *) "-output-level debug"
                   };
    int argc = sizeof(args) / sizeof(char *);
    m_subject = new TestHomeApplication(argc, args);
    QCoreApplication::processEvents();
    // in non-upstart mode, dbus-signal is sent and process is NOT stopped
    QCOMPARE(signalPid, 0);
    QCOMPARE(signalValue, 0);
    compareDbusValues();
}

void Ut_HomeApplication::testUseMode()
{
    testContextItem->setValue("");
    QVERIFY(sinkSetEnabled[homeNotificationSink]);
    QVERIFY(sinkSetEnabled[duiCompositorNotificationSink]);
    QVERIFY(sinkSetEnabled[duiFeedbackNotificationSink]);
    testContextItem->setValue("recording");
    QVERIFY(!sinkSetEnabled[homeNotificationSink]);
    QVERIFY(!sinkSetEnabled[duiCompositorNotificationSink]);
    QVERIFY(!sinkSetEnabled[duiFeedbackNotificationSink]);
}

void Ut_HomeApplication::testX11EventFilterWithPropertyNotify()
{
    WindowListReceiver r;
    connect(m_subject, SIGNAL(windowListUpdated(const QList<WindowInfo> &)), &r, SLOT(windowListUpdated(const QList<WindowInfo> &)));

    // Verify that X11EventFilter only reacts to events it's supposed to react on (type, window and atom are all set correctly)
    XEvent event;
    event.type = 0;
    event.xproperty.window = 0;
    event.xproperty.atom = 0;
    QVERIFY(!m_subject->testX11EventFilter(&event));
    event.type = PropertyNotify;
    QVERIFY(!m_subject->testX11EventFilter(&event));
    event.xproperty.window = DefaultRootWindow(QX11Info::display());
    QVERIFY(!m_subject->testX11EventFilter(&event));
    event.type = 0;
    QVERIFY(!m_subject->testX11EventFilter(&event));
    event.xproperty.atom = X11Wrapper::XInternAtom(QX11Info::display(), "_NET_CLIENT_LIST", False);
    QVERIFY(!m_subject->testX11EventFilter(&event));
    event.xproperty.window = 0;
    QVERIFY(!m_subject->testX11EventFilter(&event));
    event.type = PropertyNotify;
    QVERIFY(!m_subject->testX11EventFilter(&event));
    event.xproperty.window = DefaultRootWindow(QX11Info::display());
    QVERIFY(m_subject->testX11EventFilter(&event));

    // Make sure the window list change signal was emitted
    QCOMPARE(r.count, 1);

    // There should be 3 windows in the window list and their information should be correct
    QCOMPARE(r.windowList.count(), 4);
    QCOMPARE(r.windowList.at(0).title(), QString("plain_x_window"));
    QCOMPARE(r.windowList.at(0).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 0));
    QCOMPARE(r.windowList.at(0).windowPriority(), WindowInfo::Normal);
    QCOMPARE(r.windowList.at(1).title(), QString("Test"));
    QCOMPARE(r.windowList.at(1).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 4));
    QCOMPARE(r.windowList.at(1).windowAttributes().width, 100 + (WINDOW_ATTRIBUTE_TEST_WINDOWS + 4));
    QCOMPARE(r.windowList.at(1).windowAttributes().height, 200 + (WINDOW_ATTRIBUTE_TEST_WINDOWS + 4));
    QCOMPARE(r.windowList.at(1).windowAttributes().c_class, InputOutput);
    QVERIFY(r.windowList.at(1).windowAttributes().map_state != IsUnmapped);
    QCOMPARE(r.windowList.at(1).icon(), (Pixmap)202);
    QCOMPARE(r.windowList.at(1).windowPriority(), WindowInfo::Normal);
    QCOMPARE(r.windowList.at(2).title(), QString("Tzzt"));
    QCOMPARE(r.windowList.at(2).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 8));
    QCOMPARE(r.windowList.at(2).windowAttributes().width, 100 + (WINDOW_ATTRIBUTE_TEST_WINDOWS + 8));
    QCOMPARE(r.windowList.at(2).windowAttributes().height, 200 + (WINDOW_ATTRIBUTE_TEST_WINDOWS + 8));
    QCOMPARE(r.windowList.at(2).windowAttributes().c_class, InputOutput);
    QCOMPARE(r.windowList.at(2).icon(), (Pixmap)303);
    QVERIFY(r.windowList.at(2).windowAttributes().map_state != IsUnmapped);
    QCOMPARE(r.windowList.at(2).windowPriority(), WindowInfo::Normal);
    QCOMPARE(r.windowList.at(3).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 9));
    QCOMPARE(r.windowList.at(3).windowPriority(), WindowInfo::Call);
    QCOMPARE(r.windowList.at(3).title(), QString("Call_ui"));

    // HomeApplication should be interested in the windows' VisibilityNotifys
    QCOMPARE(visibilityNotifyWindows.count(), 4);
    QCOMPARE(visibilityNotifyWindows.at(0), r.windowList.at(0).window());
    QCOMPARE(visibilityNotifyWindows.at(1), r.windowList.at(1).window());
    QCOMPARE(visibilityNotifyWindows.at(2), r.windowList.at(2).window());
    QCOMPARE(visibilityNotifyWindows.at(3), r.windowList.at(3).window());
}

void Ut_HomeApplication::testX11EventFilterWithVisibilityNotify()
{
    MainWindow *w = MainWindow::instance(true);
    QList<DuiWindow *> windowList;
    windowList.append(w);
    gDuiApplicationStub->stubSetReturnValue<QList<DuiWindow *> >("windows", windowList);

    int x11EventFilterCallCount;
    WindowVisibilityReceiver r;
    connect(m_subject, SIGNAL(windowVisibilityChanged(Window)), &r, SLOT(windowVisibilityChanged(Window)));

    // Verify that X11EventFilter only reacts to events it's supposed to react on
    XEvent event;
    event.type = VisibilityNotify;
    event.xvisibility.window = 303;
    event.xvisibility.state = VisibilityUnobscured;
    event.xvisibility.send_event = TRUE;

    // Make sure the window visibility change signal is not emitted if state is VisibilityUnobscured
    x11EventFilterCallCount = gDuiApplicationStub->stubCallCount("x11EventFilter");
    QVERIFY(!m_subject->testX11EventFilter(&event));
    QCOMPARE(r.windowList.count(), 0);
    QCOMPARE(gDuiApplicationStub->stubCallCount("x11EventFilter"), x11EventFilterCallCount + 1);

    // Make sure the window visibility change signal was emitted with the given window if state is VisibilityFullyObscured
    x11EventFilterCallCount = gDuiApplicationStub->stubCallCount("x11EventFilter");
    event.xvisibility.state = VisibilityFullyObscured;
    QVERIFY(m_subject->testX11EventFilter(&event));
    QCOMPARE(r.windowList.count(), 1);
    QCOMPARE(r.windowList.at(0), (Window)303);
    QCOMPARE(gDuiApplicationStub->stubCallCount("x11EventFilter"), x11EventFilterCallCount);

    // Make sure the window visibility change signal is not emitted if send_event is FALSE but the event is processed
    event.xvisibility.send_event = FALSE;
    QVERIFY(m_subject->testX11EventFilter(&event));
    QCOMPARE(r.windowList.count(), 1);
    QCOMPARE(gDuiApplicationStub->stubCallCount("x11EventFilter"), x11EventFilterCallCount);

    // Make sure the window visibility change signal is not emitted if the window is the homescreen itself
    event.xvisibility.window = w->winId();
    QVERIFY(!m_subject->testX11EventFilter(&event));
    QCOMPARE(r.windowList.count(), 1);
    QCOMPARE(gDuiApplicationStub->stubCallCount("x11EventFilter"), x11EventFilterCallCount + 1);
}

void Ut_HomeApplication::testX11EventFilterWithClientMessage()
{
    WindowListReceiver r;
    connect(m_subject, SIGNAL(windowListUpdated(const QList<WindowInfo> &)), &r, SLOT(windowListUpdated(const QList<WindowInfo> &)));

    // Verify that X11EventFilter only reacts to events it's supposed to react on (type and message_type are both set correctly)
    XEvent clientEvent;
    clientEvent.type = 0;
    clientEvent.xclient.message_type = 0;
    QVERIFY(!m_subject->testX11EventFilter(&clientEvent));
    clientEvent.type = ClientMessage;
    QVERIFY(!m_subject->testX11EventFilter(&clientEvent));
    clientEvent.type = 0;
    clientEvent.xclient.message_type = X11Wrapper::XInternAtom(QX11Info::display(), "_NET_CLOSE_WINDOW", False);
    QVERIFY(!m_subject->testX11EventFilter(&clientEvent));
    clientEvent.type = ClientMessage;
    clientEvent.xclient.window = WINDOW_ATTRIBUTE_TEST_WINDOWS + 8;
    QVERIFY(m_subject->testX11EventFilter(&clientEvent));

    // Make sure the window list change signal was emitted
    QCOMPARE(r.count, 1);

    // There should be 3 windows in the window list
    QCOMPARE(r.windowList.count(), 3);
    QCOMPARE(r.windowList.at(0).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 0));
    QCOMPARE(r.windowList.at(1).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 4));
    QCOMPARE(r.windowList.at(2).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 9));

    // Change the client list so that the window being closed was actually closed
    clientListNumberOfWindows = WINDOW_ATTRIBUTE_TEST_WINDOWS + 4;
    XEvent propertyEvent;
    propertyEvent.type = PropertyNotify;
    propertyEvent.xproperty.window = DefaultRootWindow(QX11Info::display());
    propertyEvent.xproperty.atom = X11Wrapper::XInternAtom(QX11Info::display(), "_NET_CLIENT_LIST", False);
    QVERIFY(m_subject->testX11EventFilter(&propertyEvent));

    // Make sure the window list change signal was emitted
    QCOMPARE(r.count, 2);

    // There should be 2 windows in the window list
    QCOMPARE(r.windowList.count(), 2);
    QCOMPARE(r.windowList.at(0).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 0));
    QCOMPARE(r.windowList.at(1).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 4));

    // Include all windows in the client list again and mark an inexisting window as a window being closed
    clientListNumberOfWindows = NUMBER_OF_WINDOWS;
    clientEvent.xclient.window = 0;
    QVERIFY(m_subject->testX11EventFilter(&clientEvent));

    // Make sure the window list change signal was emitted
    QCOMPARE(r.count, 3);

    // There should be 3 windows in the window list
    QCOMPARE(r.windowList.count(), 4);
    QCOMPARE(r.windowList.at(0).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 0));
    QCOMPARE(r.windowList.at(1).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 4));
    QCOMPARE(r.windowList.at(2).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 8));
    QCOMPARE(r.windowList.at(3).window(), (Window)(WINDOW_ATTRIBUTE_TEST_WINDOWS + 9));
}

void Ut_HomeApplication::testContentSearchLaunch()
{
    // Add interface so that isValid() returns true
    validInterfaces.append(QString("com.nokia.ContentSearchIf"));

    // Launch the content search service
    m_subject->launchContentSearchService();

    // Verify that content search is launched
    QCOMPARE(serviceInterfaces.count(), 1);
    QCOMPARE(serviceInterfaces.at(0), QString("com.nokia.ContentSearchIf"));
    QCOMPARE(asyncCallMethods.at(0), QString("launch"));
    QCOMPARE(asyncCallArguments.at(0).count(), 1);
    QCOMPARE(asyncCallArguments.at(0).at(0).type(), QVariant::String);
    QCOMPARE(asyncCallArguments.at(0).at(0).toString(), QString(""));
}

void Ut_HomeApplication::testContentSearchLaunchWithoutServiceFW()
{
    // Click the search button
    m_subject->launchContentSearchService();

    // Verify that home application doesn't try to launch the service
    QCOMPARE(serviceInterfaces.count(), 1);
    QCOMPARE(serviceInterfaces.at(0), QString("com.nokia.ContentSearchIf"));
    QCOMPARE(asyncCallMethods.count(), 0);
}

void Ut_HomeApplication::testUpdateWindowList()
{
    WindowListReceiver r;
    connect(m_subject, SIGNAL(windowListUpdated(const QList<WindowInfo> &)), &r, SLOT(windowListUpdated(const QList<WindowInfo> &)));

    // Verify that a window list change was emitted when the application was visible.
    XEvent event;
    event.type = PropertyNotify;
    event.xproperty.atom = X11Wrapper::XInternAtom(QX11Info::display(), "_NET_CLIENT_LIST", False);
    event.xproperty.window = DefaultRootWindow(QX11Info::display());
    QVERIFY(m_subject->testX11EventFilter(&event));

    // Make sure the window list change signal was emitted
    QCOMPARE(r.count, 1);
}

QTEST_APPLESS_MAIN(Ut_HomeApplication)