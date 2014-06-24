#include "group.h"
#include "groupwidget.h"
#include "groupchatform.h"
#include "friendlist.h"
#include "friend.h"
#include "widget.h"
#include "core.h"
#include <QDebug>

Group::Group(int GroupId, QString Name)
    : groupId(GroupId), nPeers{0}, hasPeerInfo{false}
{
    widget = new GroupWidget(groupId, Name);
    chatForm = new GroupChatForm(this);
    connect(&peerInfoTimer, SIGNAL(timeout()), this, SLOT(queryPeerInfo()));
    peerInfoTimer.setInterval(500);
    peerInfoTimer.setSingleShot(false);
    //peerInfoTimer.start();
}

Group::~Group()
{
    delete chatForm;
    delete widget;
}

void Group::queryPeerInfo()
{
    const Core* core = Widget::getInstance()->getCore();
    int nPeersResult = core->getGroupNumberPeers(groupId);
    if (nPeersResult == -1)
    {
        qDebug() << "Group::queryPeerInfo: Can't get number of peers";
        return;
    }
    nPeers = nPeersResult;
    widget->onUserListChanged();
    chatForm->onUserListChanged();

    if (nPeersResult == 0)
        return;

    bool namesOk = true;
    QList<QString> names = core->getGroupPeerNames(groupId);
    if (names.isEmpty())
    {
        qDebug() << "Group::queryPeerInfo: Can't get names of peers";
        return;
    }
    for (int i=0; i<names.size(); i++)
    {
        QString name = names[i];
        if (name.isEmpty())
        {
            name = "<Unknown>";
            namesOk = false;
        }
        peers[i] = name;
    }
    nPeers = names.size();

    widget->onUserListChanged();
    chatForm->onUserListChanged();

    if (namesOk)
    {
        qDebug() << "Group::queryPeerInfo: Successfully loaded names";
        hasPeerInfo = true;
        peerInfoTimer.stop();
    }
}

void Group::addPeer(int peerId, QString name)
{
    if (peers.contains(peerId))
        qWarning() << "Group::addPeer: peerId already used, overwriting anyway";
    if (name.isEmpty())
        peers[peerId] = "<Unknown>";
    else
        peers[peerId] = name;
    nPeers++;
    widget->onUserListChanged();
    chatForm->onUserListChanged();
}

void Group::removePeer(int peerId)
{
    peers.remove(peerId);
    widget->onUserListChanged();
    chatForm->onUserListChanged();
}

void Group::updatePeer(int peerId, QString name)
{
    peers[peerId] = name;
    widget->onUserListChanged();
    chatForm->onUserListChanged();
}
