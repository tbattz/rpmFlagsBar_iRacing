//
// Created by tbatt on 8/06/2020.
//

#include "IAction.h"

IAction::IAction(const char *actionName) {
    this->actionName = actionName;
}

bool IAction::isAvailable() {
    return available;
}
