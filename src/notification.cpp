/*
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Vesa Halttunen <vesa.halttunen@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */
#include "notificationmanagerproxy.h"
#include "notification.h"

const char *HINT_CATEGORY = "category";
const char *HINT_ITEM_COUNT = "x-nemo-item-count";
const char *HINT_TIMESTAMP = "x-nemo-timestamp";
const char *HINT_PREVIEW_BODY = "x-nemo-preview-body";
const char *HINT_PREVIEW_SUMMARY = "x-nemo-preview-summary";
const char *HINT_REMOTE_ACTION = "x-nemo-remote-action-default";

static inline QString appName() {
    return QFileInfo(QCoreApplication::arguments()[0]).fileName();
}

//! A proxy for accessing the notification manager
Q_GLOBAL_STATIC_WITH_ARGS(NotificationManagerProxy, notificationManagerProxyInstance, ("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus()))

NotificationManagerProxy *notificationManager()
{
    if (!notificationManagerProxyInstance.exists()) {
        qDBusRegisterMetaType<NotificationData>();
        qDBusRegisterMetaType<QList<NotificationData> >();
    }

    return notificationManagerProxyInstance();
}

NotificationData::NotificationData()
    : replacesId(0)
{
}

/*!
    \qmltype Notification
    \brief Allows notifications to be published

    The Notification class is a convenience class for using notifications
    based on the
    \l {http://www.galago-project.org/specs/notification/0.9/} {Desktop
    Notifications Specification} as implemented in Nemo.

    Since the Nemo implementation allows static notification parameters,
    such as icon, urgency, priority and user closability definitions, to
    be defined as part of notification category definitions, this
    convenience class is kept as simple as possible by allowing only the
    dynamic parameters, such as summary and body text, item count and
    timestamp to be defined. Other parameters should be defined in the
    notification category definition. Please refer to Lipstick documentation
    for a complete description of the category definition files.

    An example of the usage of this class from a Qml application:

    \qml
    Button {
        Notification {
            id: notification
            category: "x-nemo.example"
            summary: "Notification summary"
            body: "Notification body"
            previewSummary: "Notification preview summary"
            previewBody: "Notification preview body"
            itemCount: 5
            timestamp: "2013-02-20 18:21:00"
            remoteDBusCallServiceName: "org.nemomobile.example"
            remoteDBusCallObjectPath: "/example"
            remoteDBusCallInterface: "org.nemomobile.example"
            remoteDBusCallMethodName: "doSomething"
            remoteDBusCallArguments: [ "argument", 1 ]
            onClicked: console.log("Clicked")
            onClosed: console.log("Closed, reason: " + reason)
        }
        text: "Application notification, ID " + notification.replacesId
        onClicked: notification.publish()
    }
    \endqml

    An example category definition file
    /usr/share/lipstick/notificationcategories/x-nemo.example.conf:

    \code
    x-nemo-icon=icon-lock-sms
    x-nemo-preview-icon=icon-s-status-sms
    x-nemo-feedback=sms
    x-nemo-priority=70
    x-nemo-user-removable=true
    x-nemo-user-closeable=false
    \endcode

    After publishing the ID of the notification can be found from the
    replacesId property.

    Notification::notifications() can be used to fetch notifications
    created by the calling application.
 */
Notification::Notification(QObject *parent) :
    QObject(parent),
    data(new NotificationData)
{
    connect(notificationManager(), SIGNAL(ActionInvoked(uint,QString)), this, SLOT(checkActionInvoked(uint,QString)));
    connect(notificationManager(), SIGNAL(NotificationClosed(uint,uint)), this, SLOT(checkNotificationClosed(uint,uint)));
    connect(this, SIGNAL(remoteDBusCallChanged()), this, SLOT(setRemoteActionHint()));
}

Notification::~Notification()
{
}

/*!
    \qmlproperty QString Notification::category

    The type of notification this is. Defaults to an empty string.
 */
QString Notification::category() const
{
    return data->hints.value(HINT_CATEGORY).toString();
}

void Notification::setCategory(const QString &category)
{
    if (category != this->category()) {
        data->hints.insert(HINT_CATEGORY, category);
        emit categoryChanged();
    }
}

/*!
    \qmlproperty uint Notification::replacesId

    The optional notification ID that this notification replaces. The server must atomically (ie with no flicker or other visual cues) replace the given notification with this one. This allows clients to effectively modify the notification while it's active. A value of value of 0 means that this notification won't replace any existing notifications. Defaults to 0.
 */
uint Notification::replacesId() const
{
    return data->replacesId;
}

void Notification::setReplacesId(uint id)
{
    if (data->replacesId != id) {
        data->replacesId = id;
        emit replacesIdChanged();
    }
}

/*!
    \qmlproperty QString Notification::summary

    The summary text briefly describing the notification. Defaults to an empty string.
 */
QString Notification::summary() const
{
    return data->summary;
}

void Notification::setSummary(const QString &summary)
{
    if (data->summary != summary) {
        data->summary = summary;
        emit summaryChanged();
    }
}

/*!
    \qmlproperty QString Notification::body

    The optional detailed body text. Can be empty. Defaults to an empty string.
 */
QString Notification::body() const
{
    return data->body;
}

void Notification::setBody(const QString &body)
{
    if (data->body != body) {
        data->body = body;
        emit bodyChanged();
    }
}

/*!
    \qmlproperty QDateTime Notification::timestamp

    The timestamp for the notification. Should be set to the time when the event the notification is related to has occurred. Defaults to current time.
 */
QDateTime Notification::timestamp() const
{
    return data->hints.value(HINT_TIMESTAMP).toDateTime();
}

void Notification::setTimestamp(const QDateTime &timestamp)
{
    if (timestamp != this->timestamp()) {
        data->hints.insert(HINT_TIMESTAMP, timestamp.toString(Qt::ISODate));
        emit timestampChanged();
    }
}

/*!
    \qmlproperty QString Notification::previewSummary

    Summary text to be shown in the preview banner for the notification, if any. Defaults to an empty string.
 */
QString Notification::previewSummary() const
{
    return data->hints.value(HINT_PREVIEW_SUMMARY).toString();
}

void Notification::setPreviewSummary(const QString &previewSummary)
{
    if (previewSummary != this->previewSummary()) {
        data->hints.insert(HINT_PREVIEW_SUMMARY, previewSummary);
        emit previewSummaryChanged();
    }
}

/*!
    \qmlproperty QString Notification::previewBody

    Body text to be shown in the preview banner for the notification, if any. Defaults to an empty string.
 */
QString Notification::previewBody() const
{
    return data->hints.value(HINT_PREVIEW_BODY).toString();
}

void Notification::setPreviewBody(const QString &previewBody)
{
    if (previewBody != this->previewBody()) {
        data->hints.insert(HINT_PREVIEW_BODY, previewBody);
        emit previewBodyChanged();
    }
}

/*!
    \qmlproperty int Notification::itemCount

    The number of items represented by the notification. For example, a single notification can represent four missed calls by setting the count to 4. Defaults to 1.
 */
int Notification::itemCount() const
{
    return data->hints.value(HINT_ITEM_COUNT).toInt();
}

void Notification::setItemCount(int itemCount)
{
    if (itemCount != this->itemCount()) {
        data->hints.insert(HINT_ITEM_COUNT, itemCount);
        emit itemCountChanged();
    }
}

/*!
    \qmlmethod void Notification::publish()

    Publishes the notification. If replacesId is 0, it will be a new
    notification. Otherwise the existing notification with the given ID
    is updated with the new details.
*/
void Notification::publish()
{
    setReplacesId(notificationManager()->Notify(appName(), data->replacesId, QString(), data->summary, data->body,
                                                (QStringList() << "default" << ""), data->hints, -1));
}

/*!
    \qmlmethod void Notification::close()

    Closes the notification if it has been published.
*/
void Notification::close()
{
    if (data->replacesId != 0) {
        notificationManager()->CloseNotification(data->replacesId);
        setReplacesId(0);
    }
}

/*!
    \qmlsignal Notification::clicked()

    Sent when the notification is clicked (its default action is invoked).
*/
void Notification::checkActionInvoked(uint id, QString actionKey)
{
    if (id == data->replacesId && actionKey == "default") {
        emit clicked();
    }
}

/*!
    \qmlsignal Notification::closed(uint reason)

    Sent when the notification has been closed.
    \a reason is 1 if the notification expired, 2 if the notification was
    dismissed by the user, 3 if the notification was closed by a call to
    CloseNotification.
*/
void Notification::checkNotificationClosed(uint id, uint reason)
{
    if (id == data->replacesId) {
        emit closed(reason);
        setReplacesId(0);
    }
}

/*!
    \qmlproperty QString Notification::remoteDBusCallServiceName

    The service name of the D-Bus call for this notification. Defaults to an empty string.
 */
QString Notification::remoteDBusCallServiceName() const
{
    return data->remoteDBusCallServiceName;
}

void Notification::setRemoteDBusCallServiceName(const QString &serviceName)
{
    if (data->remoteDBusCallServiceName != serviceName) {
        data->remoteDBusCallServiceName = serviceName;
        emit remoteDBusCallChanged();
    }
}

/*!
    \qmlproperty QString Notification::remoteDBusCallObjectPath

    The object path of the D-Bus call for this notification. Defaults to an empty string.
 */
QString Notification::remoteDBusCallObjectPath() const
{
    return data->remoteDBusCallObjectPath;
}

void Notification::setRemoteDBusCallObjectPath(const QString &objectPath)
{
    if (data->remoteDBusCallObjectPath != objectPath) {
        data->remoteDBusCallObjectPath = objectPath;
        emit remoteDBusCallChanged();
    }
}

/*!
    \qmlproperty QString Notification::remoteDBusCallInterface

    The interface of the D-Bus call for this notification. Defaults to an empty string.
 */
QString Notification::remoteDBusCallInterface() const
{
    return data->remoteDBusCallInterface;
}

void Notification::setRemoteDBusCallInterface(const QString &interface)
{
    if (data->remoteDBusCallInterface != interface) {
        data->remoteDBusCallInterface = interface;
        emit remoteDBusCallChanged();
    }
}

/*!
    \qmlproperty QString Notification::remoteDBusCallMethodName

    The method name of the D-Bus call for this notification. Defaults to an empty string.
 */
QString Notification::remoteDBusCallMethodName() const
{
    return data->remoteDBusCallMethodName;
}

void Notification::setRemoteDBusCallMethodName(const QString &methodName)
{
    if (data->remoteDBusCallMethodName != methodName) {
        data->remoteDBusCallMethodName = methodName;
        emit remoteDBusCallChanged();
    }
}

/*!
    \qmlproperty QVariantList Notification::remoteDBusCallArguments

    The arguments of the D-Bus call for this notification. Defaults to an empty variant list.
 */
QVariantList Notification::remoteDBusCallArguments() const
{
    return data->remoteDBusCallArguments;
}

void Notification::setRemoteDBusCallArguments(const QVariantList &arguments)
{
    if (data->remoteDBusCallArguments != arguments) {
        data->remoteDBusCallArguments = arguments;
        emit remoteDBusCallChanged();
    }
}

void Notification::setRemoteActionHint()
{
    QString s;

    if (!data->remoteDBusCallServiceName.isEmpty()
        && !data->remoteDBusCallObjectPath.isEmpty()
        && !data->remoteDBusCallInterface.isEmpty()
        && !data->remoteDBusCallMethodName.isEmpty()) {
        s.append(data->remoteDBusCallServiceName).append(' ');
        s.append(data->remoteDBusCallObjectPath).append(' ');
        s.append(data->remoteDBusCallInterface).append(' ');
        s.append(data->remoteDBusCallMethodName);

        foreach(const QVariant &arg, data->remoteDBusCallArguments) {
            // Serialize the QVariant into a QBuffer
            QBuffer buffer;
            buffer.open(QIODevice::ReadWrite);
            QDataStream stream(&buffer);
            stream << arg;
            buffer.close();

            // Encode the contents of the QBuffer in Base64
            s.append(' ');
            s.append(buffer.buffer().toBase64().data());
        }
    }

    data->hints.insert(HINT_REMOTE_ACTION, s);
}

/*!
    \fn QVariant Notification::hintValue(const QString &hint) const

    Returns the value of the given \a hint .
*/
QVariant Notification::hintValue(const QString &hint) const
{
    return data->hints.value(hint);
}

/*!
    \fn void Notification::setHintValue(const QString &hint, const QVariant &value)

    Sets the value of the given \a hint to a given \a value .
*/
void Notification::setHintValue(const QString &hint, const QVariant &value)
{
    data->hints.insert(hint, value);
}

/*!
    \qmlmethod void Notification::notifications()

    Returns a list of notifications created by the calling application.
    The returned objects are Notification components. They are only destroyed
    when the application is closed, so the caller should take their ownership
    and destroy them when they are not used anymore.
*/
QList<QObject*> Notification::notifications()
{
    QList<NotificationData> notifications = notificationManager()->GetNotifications(appName());
    QList<QObject*> objects;
    foreach (const NotificationData &notification, notifications) {
        objects.append(createNotification(notification, notificationManager()));
    }
    return objects;
}

Notification * Notification::createNotification(const NotificationData &data, QObject *parent)
{
    Notification *notification = new Notification(parent);
    notification->data->replacesId = data.replacesId;
    notification->data->summary = data.summary;
    notification->data->body = data.body;
    notification->data->hints = data.hints;
    notification->data->remoteDBusCallServiceName = data.remoteDBusCallServiceName;
    notification->data->remoteDBusCallObjectPath = data.remoteDBusCallObjectPath;
    notification->data->remoteDBusCallInterface = data.remoteDBusCallInterface;
    notification->data->remoteDBusCallMethodName = data.remoteDBusCallMethodName;
    notification->data->remoteDBusCallArguments = data.remoteDBusCallArguments;
    return notification;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NotificationData &notification)
{
    argument.beginStructure();
    argument << appName();
    argument << notification.replacesId;
    argument << QString();
    argument << notification.summary;
    argument << notification.body;
    argument << (QStringList() << "default" << "");
    argument << notification.hints;
    argument << -1;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NotificationData &notification)
{
    QString tempString;
    QStringList tempStringList;
    int tempInt;

    argument.beginStructure();
    argument >> tempString;
    argument >> notification.replacesId;
    argument >> tempString;
    argument >> notification.summary;
    argument >> notification.body;
    argument >> tempStringList;
    argument >> notification.hints;
    argument >> tempInt;
    argument.endStructure();
    return argument;
}
