![prior_logo_small2.png](https://bitbucket.org/repo/p4KM8eo/images/3027886167-prior_logo_small2.png)
# README #

Summary:
Prior is an embedded Real-Time Operating System with a small footprint 
and high customizability enabling it to run smoothly on even the smallest 
devices such as Atmel’s  ATMega series. This combined with the fact that 
the kernel is entirely written in C99 makes it portable to almost any 
microcontroller or microprocessor architecture. With Prior RTOS the 
development of event driven real-time applications becomes a painless 
process using the wide variety of intuitive API functions.

Current version: V0.4.0

Features and Specifications:

* Cooperative scheduler based on weighted FIFO.
* Priority system with 4 categories to indicate scheduling/timing constraints and a priority level ranging from 1 through 5.
* Inter-Task Communication (ITC) system providing pipes, mailboxes, semaphores, mutexes and ringbuffers.
* Global Event System 
* Advanced memory management
* OS Tick overhead: ±750 clock-cycles (on AVR8).
* Small footprint kernel with a minimal size of 15 kB ROM and 350 Bytes of RAM (O0).
* Configurable modules and peripherals to tailor the OS to your application.

Planned:

* Integrated Command Shell accessible via UART connection. (V 0.4.1)
* ITC: Messaging. (V 0.4.1)
* Pre-emptive scheduling algorithm to enable multi-tasking and instant response to events. (V 0.5.0)
* 32 Asynchronous Signals (of which 24 configurable by the user) with priority level ranging from 0 to 15. (V 0.5.1)
* TaskTrace enables the developer to trace every task, timer, event and action to speed up the debugging process. (V 0.5.2)
* Integrated Graphical User Interface framework for fast and easy GUI development. (V 0.5.3)
* Garbage Collector. (V 0.5.4)

Contact information:
D. van de Spijker <spijker.dorus@gmail.com>