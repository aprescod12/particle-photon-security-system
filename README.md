# particle-photon-security-system

## Project originally completed in Spring 2024 and published to GitHub in 2026 for portfolio documentation.

## Overview
This project implements a real-time IoT burglar alarm system using a Particle Photon microcontroller. The system is designed around a finite state machine (FSM) architecture and integrates interrupt-driven inputs, hardware timers, and cloud connectivity to simulate a practical embedded security device.

Development and deployment were performed using Particle’s Web IDE (build.particle.io), leveraging Particle Device OS and cloud APIs.

## System Features

Finite State Machine–based control logic

Interrupt-driven button input for immediate user interaction

Hardware timer–driven system tick (200 ms) for deterministic state updates

Light-sensor–based motion detection

Audible alarm using a piezo buzzer

Visual feedback using onboard and external LEDs

Cloud integration for:

Remote system activation via Particle.function

Motion alert publishing via Particle.publish

## Finite State Machine Design

The system operates through the following states:

OFF – System inactive

ARMING – Countdown before system is armed (visual feedback enabled)

ARMED – System monitoring for motion

DETECTION – Motion detected; verification period

ALARM – Audible and visual alarm triggered; cloud alert published

RESET – Cooldown period before re-arming

This architecture ensures clear separation of system behavior and enables predictable, testable transitions between operational modes.

## Hardware Components

Particle Photon microcontroller

Push button (user input)

Light sensor (motion detection via light change)

Piezo buzzer

Onboard LED (status indication)

External LED (arming/alarm indication)

## Software Architecture

Language: C++ (Particle Device OS)

Development Environment: Particle Web IDE (build.particle.io)

Timing Model:

Hardware timer interrupt every 200 ms

FSM evaluated on each timer tick

Event Handling:

Button input via external interrupt

Cloud-based control and alerts via Particle APIs

## Cloud Integration

The system integrates with the Particle Cloud to enable remote interaction:

Remote Activation:
A secure command sent via Particle.function allows the alarm to be armed from the cloud.

Motion Alerts:
When an alarm condition is confirmed, a "Motion Detected" event is published using Particle.publish, enabling notifications, logging, or downstream integrations.

## Project Context

This project was completed in Spring 2024 as a comprehensive final project for an embedded systems course. It reflects an emphasis on systems-level design, real-time behavior, and IoT-oriented architecture, rather than simple polling-based microcontroller logic.

The repository was created in 2026 to formally document the project and include it as part of a professional engineering portfolio.
