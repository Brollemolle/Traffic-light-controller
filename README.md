>**Disclaimer**
>
>Most of the code has been created by configuring a project for the Nucleo-L476RG Development board in STM32CubeIDE's GUI. This includes drivers for SPI and FreeRTOS initialization.
>
>The content worth looking at that has been written by me is located within files `Core/src/freertos.c` (Only during "User code begin" segments), `Core/src/lights_functions.c`, and `Core/src/lights_test.c`. 

---

# Traffic Light Controller 🚦

Welcome to the **Traffic Light Controller** project! This project was developed as part of the **Embedded Systems** course at **KTH Royal Institute of Technology**. It implements a traffic light control system using the **STM32 NUCLEO-L476RG** development board and a custom traffic light shield designed by the course examiner.


## 📖 Project Overview

The goal of this project is to simulate a traffic light system for a road intersection, including pedestrian crossings. The system is designed to handle multiple traffic scenarios, such as car movements, pedestrian crossings, and traffic light transitions, while ensuring safety and efficiency.

The project was developed in **STM32CubeIDE** and uses **FreeRTOS** for task scheduling and real-time operations. The hardware setup includes:
- **STM32 NUCLEO-L476RG** development board.
- Custom traffic light shield (designed by the course examiner).
- Switches to simulate cars and pedestrian buttons.


## 🛠️ Features

- **Traffic Light Control**:
  - Manages traffic lights for cars and pedestrians.
  - Supports transitions between red, orange, and green lights.
  - Handles pedestrian crossing requests.

- **Pedestrian Crossing**:
  - Pedestrians can request to cross by pressing a button.
  - Traffic lights for cars are adjusted to allow safe crossing.

- **Real-Time Operation**:
  - Uses **FreeRTOS** for task scheduling and real-time behavior.
  - Ensures timely transitions and responses to user inputs.


## 🚀 How to Run the Project

### Prerequisites
- **STM32CubeIDE** installed on your system.
- **STM32 NUCLEO-L476RG** development board.
- Custom traffic light shield (provided by the course examiner).
- USB cable for power and debugging.

### Steps
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/Brollemolle/Traffic-light-controller.git
   cd Traffic-light-controller
   ```

2. **Open the Project in STM32CubeIDE**:
   - Launch STM32CubeIDE.
   - Import the project by selecting `File > Import > General > Existing Projects into Workspace`.
   - Browse to the cloned repository and select the project.

3. **Build the Project**:
   - Click on the hammer icon or select `Project > Build All` to compile the code.

4. **Run the Project**:
   - Connect the NUCLEO-L476RG board to your PC via USB.
   - Click the `Debug` or `Run` button in STM32CubeIDE to flash the program to the board.

5. **Test the System**:
   - Use the switches on the traffic light shield to simulate cars.
   - Press the pedestrian buttons to request crossings.
   - Observe the traffic light behavior and ensure it meets the requirements.


## 📂 Project Structure

```
Traffic-light-controller/
├── Core/               # Core application code
│   ├── Inc/            # Header files
│   ├── Src/            # Source files
│   └── FreeRTOS/       # FreeRTOS configuration and tasks
├── Drivers/            # STM32 HAL drivers
├── STM32CubeIDE/       # STM32CubeIDE project files
├── README.md           # This file
└── Documentation/      # Project-related documents (if any)
```


## 📄 Project Documents

For more details about the project requirements and implementation tasks, refer to the following document:
- [Project Task Description](IS1300_Project_2024.pdf)


## 🧠 What I Learned

- **Embedded Systems Development**:
  - Working with STM32 microcontrollers and STM32CubeIDE.
  - Using FreeRTOS for real-time task scheduling.

- **Hardware Integration**:
  - Interfacing with custom hardware (traffic light shield).
  - Debugging and testing on physical hardware.

- **Software Design**:
  - Modular and maintainable code structure.
  - Implementing state machines for traffic light control.


## 🤝 Contributing

This project was developed as part of a course assignment, so contributions are not expected.

## 📜 License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.


## 🙏 Acknowledgments

- **KTH Royal Institute of Technology** for providing the course and resources.
- The course examiner for designing the custom traffic light shield.
- The **FreeRTOS** community for their excellent real-time operating system.
