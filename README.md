# README #

![Optional Text](../master/logos/prior_logo_small2.png)

Summary:
Prior is an embedded Real-Time Operating System with a small footprint 
and high customizability enabling it to run smoothly on smaller 
devices such as Atmel’s ATMega series. This combined with the fact that 
the kernel is entirely written in C99 makes it portable to almost any 
microcontroller or microprocessor architecture. With Prior RTOS the 
development of event driven real-time applications becomes a painless 
process using the wide variety of intuitive API functions.

Current version: V0.4.0

Features and Specifications:

* Cooperative scheduler based on weighted FIFO.
* Priority system with 4 categories to indicate scheduling/timing constraints and a priority level ranging from 1 through 5.
* Inter-Task Communication (ITC) system providing pipes, mailboxes, semaphores, mutexes and ringbuffers.
* Global Event System. 
* Modular architecture, custom modules can be added.
* Advanced memory management.
* OS Tick overhead: ±750 clock-cycles (on AVR8).
* Small footprint kernel with a minimal size of 15 kB ROM and 350 Bytes of RAM (O0).
* Configurable modules and peripherals to tailor the OS to your application.

Planned:

* Integrated Command Shell accessible via UART connection.
* ITC: Messaging. 
* Pre-emptive scheduling algorithm to enable multi-tasking and instant response to events.
* Asynchronous Signals. 
* TaskTrace; enables the developer to trace every task, timer, event and action to speed up the debugging process. 

Contact information:
D. van de Spijker <spijker.dorus@gmail.com>
