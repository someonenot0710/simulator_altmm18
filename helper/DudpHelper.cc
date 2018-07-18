/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "DudpHelper.h"
//#include "ns3/udp-echo-server.h"
//#include "ns3/udp-echo-client.h"
#include "ns3/DudpClient.h"
#include "ns3/DudpServer.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

DudpServerHelper::DudpServerHelper (uint16_t port)
{
  m_factory.SetTypeId (DudpServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void 
DudpServerHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
DudpServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DudpServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DudpServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
DudpServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<DudpServer> ();
  node->AddApplication (app);

  return app;
}

DudpClientHelper::DudpClientHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (DudpClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

DudpClientHelper::DudpClientHelper (Address address)
{
  m_factory.SetTypeId (DudpClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
}

void 
DudpClientHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

void
DudpClientHelper::SetFill (Ptr<Application> app, std::string fill)
{
  app->GetObject<DudpClient>()->SetFill (fill);
}

void
DudpClientHelper::SetFill (Ptr<Application> app, uint8_t fill, uint32_t dataLength)
{
  app->GetObject<DudpClient>()->SetFill (fill, dataLength);
}

void
DudpClientHelper::SetFill (Ptr<Application> app, uint8_t *fill, uint32_t fillLength, uint32_t dataLength)
{
  app->GetObject<DudpClient>()->SetFill (fill, fillLength, dataLength);
}

ApplicationContainer
DudpClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DudpClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DudpClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
DudpClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<DudpClient> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
