/*  YUView - YUV player with advanced analytics toolset
*   Copyright (C) 2015  Institut für Nachrichtentechnik
*                       RWTH Aachen University, GERMANY
*
*   YUView is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   YUView is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with YUView.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "playlistItem.h"

unsigned int playlistItem::idCounter = 0;

playlistItem::playlistItem(QString itemNameOrFileName, playlistItemType type)
{
  setName(itemNameOrFileName);
  setType(type);
  cachingEnabled = false;

  // Whenever a playlistItem is created, we give it an ID (which is unique for this instance of YUView)
  id = idCounter++;
  playlistID = -1;

  // Default values for an playlistItem_Indexed
  frameRate = DEFAULT_FRAMERATE;
  sampling  = 1;
  startEndFrame = indexRange(-1,-1);
  startEndFrameChanged = false;

  // Default duration for a playlistItem_static
  duration = PLAYLISTITEMTEXT_DEFAULT_DURATION;
}

playlistItem::~playlistItem()
{
  // If we have children delete them first
  for (int i = 0; i < childCount(); i++)
  {
    playlistItem *plItem = dynamic_cast<playlistItem*>(QTreeWidgetItem::takeChild(0));
    delete plItem;
  }
}

QList<playlistItem*> playlistItem::getItemAndAllChildren()
{
  QList<playlistItem*> returnList;
  returnList.append(this);
  for (int i = 0; i < childCount(); i++)
  {
    playlistItem *childItem = dynamic_cast<playlistItem*>(child(i));
    if (childItem)
      returnList.append(childItem->getItemAndAllChildren());
  }
  return returnList;
}

void playlistItem::setType(playlistItemType newType)
{
  if (ui.created())
  {
    // Show/híde the right controls
    bool showIndexed = (newType == playlistItem_Indexed);
    ui.labelStart->setVisible(showIndexed);
    ui.startSpinBox->setVisible(showIndexed);
    ui.labelEnd->setVisible(showIndexed);
    ui.endSpinBox->setVisible(showIndexed);
    ui.labelRate->setVisible(showIndexed);
    ui.rateSpinBox->setVisible(showIndexed);
    ui.labelSampling->setVisible(showIndexed);
    ui.samplingSpinBox->setVisible(showIndexed);

    bool showStatic  = (newType == playlistItem_Static);
    ui.durationLabel->setVisible(showStatic);
    ui.durationSpinBox->setVisible(showStatic);
  }

  type = newType;
}

// For an indexed item we save the start/end, sampling and frame rate to the playlist
void playlistItem::appendPropertiesToPlaylist(QDomElementYUView &d)
{
  // Append the playlist item properties
  d.appendProperiteChild("id", QString::number(id));

  if (type == playlistItem_Indexed)
  {
    // Append the items of the static or dynamic item
    d.appendProperiteChild( "startFrame", QString::number(startEndFrame.first) );
    d.appendProperiteChild( "endFrame", QString::number(startEndFrame.second) );
    d.appendProperiteChild( "sampling", QString::number(sampling) );
    d.appendProperiteChild( "frameRate", QString::number(frameRate) );
  }
  else
  {
    d.appendProperiteChild( "duration", QString::number(duration) );
  }
}

// Load the start/end frame, sampling and frame rate from playlist
void playlistItem::loadPropertiesFromPlaylist(QDomElementYUView root, playlistItem *newItem)
{
  newItem->playlistID = root.findChildValue("id").toInt();

  if (newItem->type == playlistItem_Indexed)
  {
    int startFrame = root.findChildValue("startFrame").toInt();
    int endFrame = root.findChildValue("endFrame").toInt();
    newItem->startEndFrame = indexRange(startFrame, endFrame);
    newItem->sampling = root.findChildValue("sampling").toInt();
    newItem->frameRate = root.findChildValue("frameRate").toInt();
  }
  else
  {
    newItem->duration = root.findChildValue("duration").toDouble();
    playlistItem::loadPropertiesFromPlaylist(root, newItem);
  }
}

void playlistItem::setStartEndFrame(indexRange range, bool emitSignal)
{
  // Set the new start/end frame (clip it first)
  indexRange startEndFrameLimit = getStartEndFrameLimits();
  startEndFrame.first = std::max(startEndFrameLimit.first, range.first);
  startEndFrame.second = std::min(startEndFrameLimit.second, range.second);

  if (!ui.created())
    // spin boxes not created yet
    return;

  if (!emitSignal)
  {
    QObject::disconnect(ui.startSpinBox, SIGNAL(valueChanged(int)), NULL, NULL);
    QObject::disconnect(ui.endSpinBox, SIGNAL(valueChanged(int)), NULL, NULL);
  }

  ui.startSpinBox->setMinimum( startEndFrameLimit.first );
  ui.startSpinBox->setMaximum( startEndFrameLimit.second );
  ui.startSpinBox->setValue( startEndFrame.first );
  ui.endSpinBox->setMinimum( startEndFrameLimit.first );
  ui.endSpinBox->setMaximum( startEndFrameLimit.second );
  ui.endSpinBox->setValue( startEndFrame.second );

  if (!emitSignal)
  {
    connect(ui.startSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotVideoControlChanged()));
    connect(ui.endSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotVideoControlChanged()));
  }
}

void playlistItem::slotVideoControlChanged()
{
  if (type == playlistItem_Static)
  {
    duration = ui.durationSpinBox->value();
  }
  else
  {
    // Was this the start or end spin box?
    QObject *sender = QObject::sender();
    if (sender == ui.startSpinBox || sender == ui.endSpinBox)
      // The user changed the start end frame
      startEndFrameChanged = true;

    // Get the currently set values from the controls
    startEndFrame.first  = ui.startSpinBox->value();
    startEndFrame.second = ui.endSpinBox->value();
    frameRate = ui.rateSpinBox->value();
    sampling  = ui.samplingSpinBox->value();

    // The current frame in the buffer is not invalid, but emit that something has changed.
    emit signalItemChanged(false, false);
  }
}

void playlistItem::slotUpdateFrameLimits()
{
  // update the spin boxes
  if (!startEndFrameChanged)
  {
    // The user did not change the start/end frame yet. If the new limits increase, we also move the startEndFrame range
    indexRange startEndFrameLimit = getStartEndFrameLimits();
    setStartEndFrame(startEndFrameLimit, false);
  }
  else
  {
    // The user did change the start/end frame. If the limits increase, keep the old range.
    setStartEndFrame(startEndFrame, false);
  }

  emit signalItemChanged(false, false);
}

QLayout *playlistItem::createPlaylistItemControls()
{
  // Absolutely always only call this function once!
  assert(!ui.created());

  ui.setupUi();

  indexRange startEndFrameLimit = getStartEndFrameLimits();
  if (startEndFrame == indexRange(-1,-1))
  {
    startEndFrame = startEndFrameLimit;
  }

  // Set min/max duration for a playlistItem_Static
  ui.durationSpinBox->setMaximum(100000);
  ui.durationSpinBox->setValue(duration);

  // Set default values for a playlistItem_Indexed
  ui.startSpinBox->setMinimum(startEndFrameLimit.first);
  ui.startSpinBox->setMaximum(startEndFrameLimit.second);
  ui.startSpinBox->setValue(startEndFrame.first);
  ui.endSpinBox->setMinimum(startEndFrameLimit.first);
  ui.endSpinBox->setMaximum(startEndFrameLimit.second);
  ui.endSpinBox->setValue(startEndFrame.second);
  ui.rateSpinBox->setMaximum(1000);
  ui.rateSpinBox->setValue(frameRate);
  ui.samplingSpinBox->setMinimum(1);
  ui.samplingSpinBox->setMaximum(100000);
  ui.samplingSpinBox->setValue(sampling);

  setType(type);

  // Connect all the change signals from the controls to "connectWidgetSignals()"
  connect(ui.startSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotVideoControlChanged()));
  connect(ui.endSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotVideoControlChanged()));
  connect(ui.rateSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotVideoControlChanged()));
  connect(ui.samplingSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotVideoControlChanged()));
  connect(ui.durationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotVideoControlChanged()));

  return ui.gridLayout;
}

void playlistItem::createPropertiesWidget()
{
  // Absolutely always only call this once// 
  assert(!propertiesWidget);

  preparePropertiesWidget(QStringLiteral("playlistItem"));

  // On the top level everything is layout vertically
  QVBoxLayout *vAllLaout = new QVBoxLayout(propertiesWidget.data());

  if (false)
  {
    // TODO FIXME this line is not in any layout and will be dangling. Do we need it?
    QFrame *line = new QFrame(propertiesWidget.data());
    line->setObjectName(QStringLiteral("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
  }

  // First add the parents controls (duration) then the text spcific controls (font, text...)
  vAllLaout->addLayout(createPlaylistItemControls());

  // Insert a stretch at the bottom of the vertical global layout so that everything
  // gets 'pushed' to the top
  vAllLaout->insertStretch(2, 1);
}

void playlistItem::preparePropertiesWidget(const QString & name)
{
  assert(!propertiesWidget);

  propertiesWidget.reset(new QWidget);
  propertiesWidget->setObjectName(name);
}