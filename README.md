
# Smart Shopping Cart

This repository contains the implementation of the **Smart Shopping Cart**, a mini-project developed as part of our academic syllabus. The project aims to enhance the shopping experience by leveraging **RFID technology** to calculate and display the total cost of selected products in real-time.

## Features
- **RFID-Based Identification**: Products are identified using RFID tags.
- **Real-Time Cost Calculation**: The total cost is displayed instantly as products are added or removed.
- **User-Friendly Interface**: A simple and intuitive interface for shoppers.
- **Efficiency**: Reduces waiting time at billing counters.

## Technology Stack
- **Hardware**:
  - RFID Reader and Tags
  - Microcontroller (e.g., ESP32/Arduino)
  - Display Module (e.g., LCD/LED)
- **Software**:
  - Programming Languages: C, Python, or Arduino IDE
  - Microcontroller Programming Libraries

## How It Works
1. Each product in the store is equipped with an RFID tag containing product details.
2. When a product is added to the cart, the RFID reader scans the tag.
3. The microcontroller processes the data and updates the total cost.
4. The updated total is displayed on the screen in real-time.
5. If a product is removed, the cost is recalculated accordingly.

## Installation and Usage
1. Clone this repository:
   ```bash
   git clone https://github.com/MJTech46/smart-shopping-cart.git
   ```
2. Assemble the hardware components as per the circuit diagram.
3. Upload the code to the microcontroller using the preferred IDE.
4. Power the system and start shopping!

## Project Goals
- Minimize manual intervention during the shopping process.
- Provide a seamless and efficient shopping experience.
- Introduce innovative technologies into traditional shopping methods.

## Contributors
- **Abin** and three others.

## License
This project is licensed under the [MIT License](LICENSE).
