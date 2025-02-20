{% include toc.html %}
## Introduction to My ePortfolio
Hello, my name is Jeremy Snow, and I appreciate you taking the time to review my ePortfolio. This ePortfolio represents the culmination of my dedication and hard work throughout my coursework at Southern New Hampshire University (SNHU). Its purpose is to showcase the skills and knowledge I have developed during my academic journey. This ePortfolio was created for the CS-499 Computer Science Capstone Course and is designed to display an artifact that has been enhanced in three different categories: Software Design and Engineering, Algorithms and Data Structures, and Databases.

This ePortfolio includes a professional self-assessment, a code review of the original artifact prior to enhancement, and a section for each of the three enhancements, accompanied by reflection narratives that provide insight into each improvement. The artifact used for all three enhancements is a game, inspired by the classic brick-breaker game Breakout, that I created in C++ as part of the CS-330: Computer Graphics and Visualization coursework. Enhancements to the game include particle effects, level selection, game-over/you-win mechanics, and a system for tracking high scores that utilizes an SQLite database to store and retrieve the necessary data. These enhancements demonstrate my expertise in software design and engineering, algorithms and data structures, and databases, aligning with the Capstone course's five core outcomes. Through these improvements, I have strengthened my ability to write efficient, secure, and well-structured code that meets industry best practices.

Thank you again for reviewing my ePortfolio. I hope it provides insight into my technical abilities, problem-solving skills, and growth as a computer scientist.

<hr>

## Professional Self-Assessment

### Introduction
My journey with SNHU began in May 2023, marking my return to completing a degree I had initially pursued years earlier. I originally started my computer science degree fresh out of high school at the University of Arkansas in 2009. After completing my sophomore year, I took a break due to the complications of life. Just over a decade later,  I resumed my studies at SNHU, determined to see it through. This program has provided me with invaluable knowledge and hands-on experience, strengthening my skills in programming languages such as C++, Java, and Python, as well as in software engineering, database management, security, and algorithm design. Each course and project I have completed has contributed to my ability to develop well-structured, efficient, and secure software solutions, preparing me to succeed in the computer science field.

### Collaboration and Teamwork
One of the most valuable aspects of my education so far has been learning how to operate effectively in a team environment. The Software Development Lifecycle course was instrumental in this regard, introducing the Agile methodologies and the importance of effective communication. This course explored Agile development roles, project management techniques, and collaboration tools, helping me understand how to work efficiently within cross-functional teams. These concepts were reinforced throughout the program, helping me develop strong collaboration skills, ensuring that I can work effectively with developers, designers, and stakeholders to achieve project goals.

### Effective Communication and Documentation
My understanding of technical communication was significantly enhanced through System Analysis and Design, where I learned how to translate complex technical concepts into clear and concise documentation, diagrams, and presentations. This course provided experience in creating business requirements documents, UML diagrams, Gantt charts, and other visual communication tools. These skills have been invaluable in ensuring that technical ideas are communicated effectively to both technical and non-technical stakeholders. By learning how to document system requirements, design architectures, and create project timelines, I have developed the ability to bridge the gap between development teams and business objectives. This experience has strengthened my ability to present ideas clearly, justify design decisions, and collaborate with diverse audiences, all of which are essential skills in a professional software development environment.

### Data Structures and Algorithms
Throughout my academic journey, I have developed a strong ability to design and implement efficient data structures and algorithms. My foundation in this area was established in the Data Structures and Algorithms: Analysis and Design course, where I explored structures such as hash tables, linked lists, binary search trees, and vectors, comparing them for efficiency and applicability in different scenarios. Through hands-on coding exercises, I gained experience in analyzing time and space complexity using Big-O notation, enabling me to write optimized software solutions. These skills have been instrumental in ensuring that the applications I develop are not only functional but also efficient, scalable, and maintainable.

### Software Engineering and Database Management
My coursework has also strengthened my expertise in software engineering and database management. The Introduction to Structured Database Environments, Full Stack Development and Client/Server Development courses taught me how to design and develop applications that integrate databases, focusing on data integrity, security, and efficiency.

- Introduction to Structured Database Environments covered the fundamentals of SQL and relational database management.
- Client/Server Development introduced me to MongoDB in a Python environment, showcasing the flexibility of NoSQL databases.
- Full Stack Development I expanded on these concepts by integrating databases into web applications, emphasizing both front-end and back-end interactions.

These database skills became particularly valuable when implementing an SQLite database for my capstone project, where I developed a high-score tracking system for my Breakout game to optimize data storage and retrieval.

### Software Security and Secure Coding
Security is a critical aspect of software development, and my coursework has given me a strong foundation in writing secure, reliable code. Courses such as Software Security, Software Testing, Automation, and Quality Assurance, and Secure Coding reinforced the importance of:

- Writing secure code that prevents vulnerabilities
- Identifying and mitigating security threats such as SQL injection, buffer overflows, and cross-site scripting
- Implementing encryption techniques to protect sensitive data
  
Software Testing, Automation, and Quality Assurance provided hands-on experience in creating test cases in a Java environment, reinforcing best practices in software validation, verification, and automated testing. Secure Coding focused on recognizing and mitigating common vulnerabilities such as SQL injection, buffer overflows, and cross-site scripting, equipping me with strategies to develop more resilient software. In Software Security, I gained a deeper understanding of advanced security concepts, including authentication, authorization, cryptography, and secure software design. The course covered threat modeling, vulnerability assessments, and best practices for protecting applications against cyber threats. Through hands-on assignments, I applied these security principles in real-world scenarios, ensuring that the software I develop is built with a security-first mindset and resistant to modern cyber threats.

### Exploring Different Areas of Computer Science
In addition to core programming concepts, I had the opportunity to explore diverse areas of computer science, broadening my understanding of the field. Courses such as:

- Mobile Architecture and Programming – Focused on Android application design, emphasizing user-centered design principles
- UI/UX Design and Development – Strengthened my ability to create intuitive, accessible, and visually appealing interfaces
- Emerging Systems Architecture and Technology – Introduced me to embedded systems development, providing insight into hardware-software integration
- Computer Graphics and Visualization – Allowed me to explore game and graphics programming, where I learned to create interactive 3D objects and implement real-time rendering techniques

These experiences helped me gain a well-rounded skill set, preparing me to adapt to different software development domains.

### Artifact Enhancement Overview and ePortfolio Summary

This ePortfolio serves as the culmination of my work throughout the Computer Science program, demonstrating my ability to design, develop, and optimize software solutions. The centerpiece of my portfolio is an enhanced Breakout-style game, originally developed in CS-330: Computer Graphics and Visualization. I have significantly improved this artifact by implementing:

- Particle effects to enhance the visual experience
- Level selection and game-over/you-win mechanics to increase playability
- High score tracking for individual levels utilizing an SQLite database

These enhancements demonstrate my proficiency in software design and engineering, algorithms and data structures, and database management, aligning with the Capstone course's five core outcomes. Additionally, my professional self-assessment highlights the technical and collaborative skills I have developed across the SNHU computer science program. Below, you will find a code review for the original artifact, prior to the three enhancements. Following the code review, a section for each enhancement will feature a narrative that explains why the artifact was selected for enhancement, how the enhancements improved it, and what I learned throughout the enhancement process. Each narrative will also reflect on the technical skills and problem-solving techniques I applied during the enhancements, as well as the challenges I faced. Through this process, I demonstrate the depth of my understanding of software development and the ability to refine and optimize code based on real-world application scenarios.

### Try it out for yourself!
The Visual Studio files for the original artifact as well as a working portable application folder are located [here](https://github.com/jjs003/CapstoneProject/tree/main/Original%20Artifact). 

In order to run this version of the game, you must first download the 'Breakout - Original Artifact' folder, then navigate to the 'x64' folder, and click on the .exe file contained inside.

## Code Review
Code reviews play a crucial role in software development, ensuring that code is efficient, maintainable, and aligned with best practices. This section includes a video walkthrough of my code review, conducted before implementing any modifications or enhancements. Through this review, I evaluate the existing functionality of my C++ game, inspired by the classic brick-breaker game Breakout, and identify areas for improvement.

A thorough code review helps catch potential issues early, improving code structure, readability, and efficiency. In my review, I analyze key aspects such as logic, security, and documentation. I highlight areas that need refinement, such as optimizing game mechanics, enhancing code organization, and improving the code comments in order to make the code more understandable for future developers. Below, you can find my initial code review for the Breakout artifact.

{% include youtube.html id="1ZgfcmOjpa4" %}

## Capstone Course Outcomes
Listed below are the outcomes that have been met through each enhancement, as well as the code review, enhancement narratives, and the professional self-assessment provided at the beginning of this ePortfolio. 

1. Employ strategies for building collaborative environments that enable diverse audiences to support organizational decision-making in the field of computer science.
2. Design, develop, and deliver professional-quality oral, written, and visual communications that are coherent, technically sound, and appropriately adapted to specific audiences and contexts.
3. Design and evaluate computing solutions that solve a given problem using algorithmic principles and computer science practices and standards appropriate to its solution, while managing the trade-offs involved in design choices.
4. Demonstrate an ability to use well-founded and innovative techniques, skills, and tools in computing practices for the purpose of implementing computer solutions that deliver value and accomplish industry-specific goals.
5. Develop a security mindset that anticipates adversarial exploits in software architecture and designs to expose potential vulnerabilities, mitigate design flaws, and ensure privacy and enhanced security of data and resources.

## [Enhancement One](https://github.com/jjs003/CapstoneProject/tree/main/EnhancementOne)

<video width="640" height="360" controls>
  <source src="https://github.com/jjs003/CapstoneProject/raw/refs/heads/main/assets/videos/EnhancementOne.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

The artifact I chose for my ePortfolio for all three enhancements is a project originally created during the coursework for CS 330: Computational Graphics and Visualization. The project began as a basic version of the game Breakout.  In its initial form, the program allowed the player to press the spacebar to release the ball, move the paddle side to side using the 'A'/left and 'D'/right keys, and break bricks with the ball. While it had the core functionality to allow the player to break all the bricks, it lacked additional logic to indicate level completion. Furthermore, when the ball passed the bottom of the screen, the level would simply restart without any further consequences or progression mechanics. This foundational version provided an excellent starting point for implementing the planned enhancements to expand the game's functionality and improve the user experience.

I selected this artifact for inclusion in my ePortfolio because it allowed me to create enhancements that demonstrate key software development skills and align closely with the course outcomes. The particle system showcases my ability to design computing solutions that balance algorithmic complexity with performance considerations (Course Outcome 3). Specifically, it highlights skills in managing dynamic visual effects, integrating shaders, and optimizing resource allocation to maintain frame rate stability. The use of industry-standard tools like OpenGL further underscores my ability to apply innovative techniques and tools (Course Outcome 4). Additionally, the enhancement reflects my ability to create visually engaging and professional-quality outputs that improve the overall user experience (Course Outcome 2). Following the completion of Enhancement One, the program’s aesthetic quality has been significantly improved, laying the groundwork for further enhancements that will continue to refine both its visual appeal and overall functionality. 

The implementation of Enhancement One was a rewarding process, and I gained significant insights along the way.  Specifically, I learned how to design and implement a particle system from scratch, including creating a particle generator class, integrating it into the game loop, and using shaders for rendering effects. A substantial amount of additional effort was required to refine the comments and formatting across existing files, which took more time than I initially anticipated. During this process, I also identified areas in the existing code where improvements could be made, enabling me to streamline the code and simplify certain algorithmic aspects for better performance and readability. One particularly frustrating issue was a file loading error that kept me stuck for nearly an hour. This was due to an improperly formatted file path, as I had forgotten to include “../” at the beginning of the file name, causing the particle texture to fail to load. Creating the particle texture and modifying the sprite used for the ball also taught me a great deal about texture design and implementation. I used GIMP to design both, and I thoroughly enjoyed exploring the program’s features and learning how to apply them effectively. This experience not only enhanced my skills in graphics creation but also deepened my understanding of how textures contribute to the overall aesthetic and functionality of a game.

### Try it out for yourself!
The Visual Studio files for this version of the artifact as well as a working portable application folder are located [here](https://github.com/jjs003/CapstoneProject/tree/main/EnhancementOne). 

In order to run this version of the game, you must first download the 'Breakout - Enhancement One' folder, then navigate to the 'x64' folder, and click on the .exe file contained inside.
