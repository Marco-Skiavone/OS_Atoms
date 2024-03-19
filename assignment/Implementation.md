# Implementation:

### Master Process
- creates resources
- creates children
- *cycle:*
    - print
    - takes energy
    - waits for 1 sec

**SIGNALS:**
- Termination (different types of termination)
- SIGALRM
---
### Atom Process
- links to resources
- receives atomic number
- *cycle:*
    - semaphore_1 wait
    - divide (***activator***)

**SIGNALS:**
- Termination (different types of termination)
- scrap (optional ?)
---
### Activator Process
- links to resources
- *cycle:*
    - `alarm(STEP_ATTIVATORE);`
    - set semaphore_1 of TOT elements *(where TOT == # of atoms dividing)*

**SIGNALS:**
- Termination (different types of termination)
- SIGALRM
---
### Power Supplier Process
- links to resources
- *cycle:*
    - waits for *X* nanosecs
    - creates *Y* atoms

**SIGNALS:**
- Termination (different types of termination)
- SIGALRM (of every *X* nanosecs)
