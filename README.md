# Internship-MEDISIP: Preliminary Measurements to Guide the Development of a Standing CT

50 SECONDS ROTATION (3000 speed in arduino)

## Code Files

* **`Study2.1.ipynb`**: Analyzes the camera tracking error and spatial distortions using the **first configuration** (ceiling-mounted).

Folders with camera on the ceiling:
| Folder Name | Condition | 
| :---: | :--- |
| **50** | 50cm from camera | 
| **70** | 70cm from camera | 
| **90** | 90cm from camera | 
| **220** | 220cm from camera | 

* **`Study2.2.ipynb`**: Analyzes the tracking error using the **second configuration** (platform-mounted) and plots participant kinematic results.

---

## Data Structure & Trials

Participant data is organized into folders named after each participant: 
`ann`, `jose`, `jens`, `mariona`, and `luca`.

Each participant folder contains data for **Trials 1 through 6**, structured as follows:

| Trial | Condition | Support Status |
| :---: | :--- | :--- |
| **1** | Eyes Open | Supported |
| **2** | Eyes Closed | Supported |
| **3** | Eyes Open | Unsupported |
| **4** | Eyes Closed | Unsupported |
| **5** | Static (20 Seconds) | Supported |
| **6** | Static (20 Seconds) | Unsupported |
| **test1** | Static | infrareds on wood (no participant)|
| **test2** | Dinamic | infrareds on wood (no participant)|

