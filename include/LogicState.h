#pragma once

// All this just replicates bool behaviour but supports tri (+error) state logic

enum class LogicState { OFF, ON, DISCONNECTED, ERROR }; 

inline LogicState logic_and(LogicState a, LogicState b) {
    if (a == LogicState::ERROR || b == LogicState::ERROR) return LogicState::ERROR;
    if (a == LogicState::OFF || b == LogicState::OFF) return LogicState::OFF;
    if (a == LogicState::DISCONNECTED || b == LogicState::DISCONNECTED) return LogicState::DISCONNECTED;
    return LogicState::ON; // both must be ON
}

inline LogicState logic_or(LogicState a, LogicState b) {
    if (a == LogicState::ERROR || b == LogicState::ERROR) return LogicState::ERROR;
    if (a == LogicState::ON || b == LogicState::ON) return LogicState::ON;
    if (a == LogicState::DISCONNECTED || b == LogicState::DISCONNECTED) return LogicState::DISCONNECTED;
    return LogicState::OFF; // both must be OFF
}

inline LogicState logic_not(LogicState a) {
    if (a == LogicState::ERROR) return LogicState::ERROR;
    if (a == LogicState::DISCONNECTED) return LogicState::DISCONNECTED;
    return (a == LogicState::ON) ? LogicState::OFF : LogicState::ON;
}


inline LogicState operator&&(LogicState a, LogicState b) {
    return logic_and(a, b);
}

inline LogicState operator||(LogicState a, LogicState b) {
    return logic_or(a, b);
}

inline LogicState operator!(LogicState a) {
    return logic_not(a);
}