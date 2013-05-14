/*
 * boardconf.h
 *
 * Author:
 *       Antonius Riha <antoniusriha@gmail.com>
 *
 * Copyright (c) 2013 Antonius Riha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef BOARDCONF_H
#define BOARDCONF_H

#include <QColor>
#include "../con4core/game.h"

class BoardConf : public QObject
{
	Q_OBJECT
public:
	static void toColorVec(QColor color, QVector<float> &colorVec);

	const static float sphereRadius = 0.1, boardBaseHeight = 0.1;

	explicit BoardConf(QObject *parent = 0);

	Game::Dimensions dims() const { return _dims; }
	void setDims(Game::Dimensions value) { _set<Game::Dimensions>(value,
																  _dims); }

	float colsDistance() const { return _colsDistance; }
	void setColsDistance(float value) { _set<float>(value, _colsDistance); }

	const float *backgroundF() const { return _background.constData(); }
	QColor background() const { return _getColor(_background); }
	void setBackground(QColor value) { _setColor(value, _background); }

	const float *boardBaseF() const { return _boardBase.constData(); }
	QColor boardBase() const { return _getColor(_boardBase); }
	void setBoardBase(QColor value) { _setColor(value, _boardBase); }

	const float *cylindersF() const { return _cylinders.constData(); }
	QColor cylinders() const { return _getColor(_cylinders); }
	void setCylinders(QColor value) { _setColor(value, _cylinders); }

	const float *player1ColorF() const { return _player1Color.constData(); }
	QColor player1Color() const { return _getColor(_player1Color); }
	void setPlayer1Color(QColor value) { _setColor(value, _player1Color); }

	const float *player2ColorF() const { return _player2Color.constData(); }
	QColor player2Color() const { return _getColor(_player2Color); }
	void setPlayer2Color(QColor value) { _setColor(value, _player2Color); }

	bool player1Enabled() const { return _player1Enabled; }
	void setPlayer1Enabled(bool value) { _set<bool>(value, _player1Enabled); }

	bool player2Enabled() const { return _player2Enabled; }
	void setPlayer2Enabled(bool value) { _set<bool>(value, _player2Enabled); }

	QString gameTitle() const { return _gameTitle; }
	void setGameTitle(QString value) { _set<QString>(value, _gameTitle); }

	QString gameDescription() const { return _gameDescription; }
	void setGameDescription(QString value) { _set<QString>(value,
														   _gameDescription); }

	QString player1Name() const { return _player1Name; }
	void setPlayer1Name(QString value) { _set<QString>(value, _player1Name); }

	QString player2Name() const { return _player2Name; }
	void setPlayer2Name(QString value) { _set<QString>(value, _player2Name); }

	bool isPlayerEnabled(FieldValue player) const;

signals:
	void changed();

private:
	template <class T>
	void _set(T value, T &field)
	{
		if (value == field) return;
		field = value;
		emit changed();
	}

	QColor _getColor(const QVector<float> color) const;
	void _setColor(QColor value, QVector<float> &field);

	Game::Dimensions _dims;
	float _colsDistance;
	QVector<float> _background, _boardBase, _cylinders,
	_player1Color, _player2Color;
	bool _player1Enabled, _player2Enabled;
	QString _gameTitle, _gameDescription, _player1Name, _player2Name;
};

#endif // BOARDCONF_H
