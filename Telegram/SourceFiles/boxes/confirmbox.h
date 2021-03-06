/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2017 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "boxes/abstractbox.h"

namespace Ui {
class Checkbox;
class FlatLabel;
} // namespace Ui

class InformBox;
class ConfirmBox : public BoxContent, public ClickHandlerHost {
public:
	ConfirmBox(QWidget*, const QString &text, base::lambda<void()> &&confirmedCallback = base::lambda<void()>(), base::lambda<void()> &&cancelledCallback = base::lambda<void()>());
	ConfirmBox(QWidget*, const QString &text, const QString &confirmText, base::lambda<void()> &&confirmedCallback = base::lambda<void()>(), base::lambda<void()> &&cancelledCallback = base::lambda<void()>());
	ConfirmBox(QWidget*, const QString &text, const QString &confirmText, const style::RoundButton &confirmStyle, base::lambda<void()> &&confirmedCallback = base::lambda<void()>(), base::lambda<void()> &&cancelledCallback = base::lambda<void()>());
	ConfirmBox(QWidget*, const QString &text, const QString &confirmText, const QString &cancelText, base::lambda<void()> &&confirmedCallback = base::lambda<void()>(), base::lambda<void()> &&cancelledCallback = base::lambda<void()>());
	ConfirmBox(QWidget*, const QString &text, const QString &confirmText, const style::RoundButton &confirmStyle, const QString &cancelText, base::lambda<void()> &&confirmedCallback = base::lambda<void()>(), base::lambda<void()> &&cancelledCallback = base::lambda<void()>());

	void updateLink();

	// If strict cancel is set the cancelledCallback is only called if the cancel button was pressed.
	void setStrictCancel(bool strictCancel) {
		_strictCancel = strictCancel;
	}

	// ClickHandlerHost interface
	void clickHandlerActiveChanged(const ClickHandlerPtr &p, bool active) override;
	void clickHandlerPressedChanged(const ClickHandlerPtr &p, bool pressed) override;

	void closeHook() override;

protected:
	void prepare() override;

	void keyPressEvent(QKeyEvent *e) override;
	void paintEvent(QPaintEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void leaveEventHook(QEvent *e) override;

private:
	struct InformBoxTag {
	};
	ConfirmBox(const InformBoxTag &, const QString &text, const QString &doneText, base::lambda_copy<void()> &&closedCallback);
	base::lambda<void()> generateInformCallback(const base::lambda_copy<void()> &closedCallback);
	friend class InformBox;

	void confirmed();
	void init(const QString &text);
	void textUpdated();

	QString _confirmText;
	QString _cancelText;
	const style::RoundButton &_confirmStyle;
	bool _informative = false;

	Text _text;
	int _textWidth = 0;
	int _textHeight = 0;

	void updateHover();

	QPoint _lastMousePos;

	bool _confirmed = false;
	bool _cancelled = false;
	bool _strictCancel = false;
	base::lambda<void()> _confirmedCallback;
	base::lambda<void()> _cancelledCallback;

};

class InformBox : public ConfirmBox {
public:
	InformBox(QWidget*, const QString &text, base::lambda_copy<void()> &&closedCallback = base::lambda_copy<void()>());
	InformBox(QWidget*, const QString &text, const QString &doneText, base::lambda_copy<void()> &&closedCallback = base::lambda_copy<void()>());

};

class MaxInviteBox : public BoxContent {
public:
	MaxInviteBox(QWidget*, const QString &link);

protected:
	void prepare() override;

	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void leaveEventHook(QEvent *e) override;

private:
	void updateSelected(const QPoint &cursorGlobalPosition);

	Text _text;
	int32 _textWidth, _textHeight;

	QString _link;
	QRect _invitationLink;
	bool _linkOver = false;

	QPoint _lastMousePos;

};

class ConvertToSupergroupBox : public BoxContent, public RPCSender {
public:
	ConvertToSupergroupBox(QWidget*, ChatData *chat);

protected:
	void prepare() override;

	void keyPressEvent(QKeyEvent *e) override;
	void paintEvent(QPaintEvent *e) override;

private:
	void convertToSupergroup();
	void convertDone(const MTPUpdates &updates);
	bool convertFail(const RPCError &error);

	ChatData *_chat;
	Text _text, _note;
	int32 _textWidth, _textHeight;

};

class PinMessageBox : public BoxContent, public RPCSender {
public:
	PinMessageBox(QWidget*, ChannelData *channel, MsgId msgId);

protected:
	void prepare() override;

	void resizeEvent(QResizeEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;

private:
	void pinMessage();
	void pinDone(const MTPUpdates &updates);
	bool pinFail(const RPCError &error);

	ChannelData *_channel;
	MsgId _msgId;

	object_ptr<Ui::FlatLabel> _text;
	object_ptr<Ui::Checkbox> _notify;

	mtpRequestId _requestId = 0;

};

class DeleteMessagesBox : public BoxContent, public RPCSender {
public:
	DeleteMessagesBox(QWidget*, HistoryItem *item, bool suggestModerateActions);
	DeleteMessagesBox(QWidget*, const SelectedItemSet &selected);

protected:
	void prepare() override;

	void resizeEvent(QResizeEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;

private:
	void deleteAndClear();

	QVector<FullMsgId> _ids;
	bool _singleItem = false;
	UserData *_moderateFrom = nullptr;
	ChannelData *_moderateInChannel = nullptr;

	object_ptr<Ui::FlatLabel> _text = { nullptr };
	object_ptr<Ui::Checkbox> _forEveryone = { nullptr };
	object_ptr<Ui::Checkbox> _banUser = { nullptr };
	object_ptr<Ui::Checkbox> _reportSpam = { nullptr };
	object_ptr<Ui::Checkbox> _deleteAll = { nullptr };

};

class ConfirmInviteBox : public BoxContent, public RPCSender {
public:
	ConfirmInviteBox(QWidget*, const QString &title, const MTPChatPhoto &photo, int count, const QVector<UserData*> &participants);

protected:
	void prepare() override;

	void resizeEvent(QResizeEvent *e) override;
	void paintEvent(QPaintEvent *e) override;

private:
	object_ptr<Ui::FlatLabel> _title;
	object_ptr<Ui::FlatLabel> _status;
	ImagePtr _photo;
	EmptyUserpic _photoEmpty;
	QVector<UserData*> _participants;

	int _userWidth = 0;

};
