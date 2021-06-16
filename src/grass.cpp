#include "grass.h"

const int Grass::type = 1;
int Grass::growing_rate = 8;

void Grass::grow() {
	if(!_grown) {
		_countdown--;
		if(_countdown == 0) {
			_grown = true;
		}
	}
}

void Grass::update(int current_rank, bool grown, int countdown) {
	id.currentRank(current_rank);
	_grown = grown;
	_countdown = countdown;
}
