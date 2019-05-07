#ifndef STATES_H
#define STATES_H

#include "gui.h"

struct StateGroup;

struct State {
	//add your own constructor

	//called when state is switched to
	virtual void enter() {}
	//called every frame
	virtual void update(vec2 mousePos) = 0;
	//called when state is switched from
	virtual void leave() {}
	//virtual destructor
	virtual ~State() {}
	std::string identifier;
	StateGroup* parent;
};

struct StateGroup {
	std::vector<State*> states;
	u32 currentState;
};

INTERNAL inline
void set_state(StateGroup& group, std::string identifier) {
	for (u16 i = 0; i < group.states.size(); ++i) {
		if (group.states[i]->identifier == identifier) {
			//leave current state
			group.states[group.currentState]->leave();
			//enter new state
			group.states[i]->enter();
			group.currentState = i;
			return;
		}
	}
	BMT_LOG(WARNING, "[%s] State doesn't exist!", identifier);
}

INTERNAL inline
void add_state(StateGroup& group, State* state, std::string identifier) {
	bool firstState = false;
	if (group.states.size() == 0) {
		firstState = true;
	}

	for (u16 i = 0; i < group.states.size(); ++i) {
		if (group.states[i]->identifier == identifier) {
			BMT_LOG(WARNING, "[%s] State already exists in current group!", identifier.c_str());
			return;
		}
	}
	state->identifier = identifier;
	state->parent = &group;
	group.states.push_back(state);

	if (firstState) {
		group.currentState = 0;
		group.states[0]->enter();
	}
}

INTERNAL inline
void remove_state(StateGroup& group, std::string identifier) {
	for (u16 i = 0; i < group.states.size(); ++i) {
		if (group.states[i]->identifier == identifier) {
			group.states.erase(group.states.begin() + i);
			return;
		}
	}
	BMT_LOG(WARNING, "[%s] State doesn't exist!", identifier);
}

INTERNAL inline
void update_current_state(StateGroup& group, vec2 mousePos) {
	group.states[group.currentState]->update(mousePos);
}

#endif