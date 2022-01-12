How to Write a MoveIt How-To Guide
===================================
This guide explains how to write How-To's for MoveIt documentation. This guide is primarily intended for use by employees at PickNik Robotics
to assist in standardizing how-to documents but can be used by any contribitor who wants to submit a new guide. If you are looking for
how-to guides for using MoveIt, you can find them here: :doc:`/doc/how_to_guides/how_to_guides`.

Learning Objectives
-------------------
- The type of information that should be included in a how-to.
- Proper formatting for how-to guides in the MoveIt documentation section.

Requirements
------------
- Ubuntu 20.04
- ROS 2 Galactic
- MoveIt2

Steps
-----
1. Fork the MoveIt2 Tutorials repository (https://github.com/ros-planning/moveit2_tutorials.git) and start a new branch with an intuitive name (e.g. jack/how-to-write-how-tos)

#. Create a new file in the ``doc/how_to_guides`` directory with a ``.rst`` extension. The title should start with "How to" followed by the specific question being answered (e.g. "How to Write a MoveIt How-To Guide").

#. Add a link to your guide on the appropriate how-to guide page:

   - User guides: :doc:`/doc/how_to_guides/how_to_guides`

   - Contributors guides: :doc:`/doc/how_to_contribute/how_to_contribute`

#. Write the introduction using reStructuredText (.rst) using the following guidelines:

   - The title should have the same name as the file.

   - The introduction should explain the purpose of this how-to and the intended audience.

   - If you think people may regularly find this particular guide by mistake, add links to the proper resource.

#. Write the specific learning objectives (i.e. what will the reader know when they finish reading this how-to).

#. Add any system or equipment requirements for this how-to so users know if this how-to is appropriate for them.

#. Layout the individual action steps and do not leave out necessary intermediate steps.

#. Create a "Further Reading" section that links to amplifying information.

#. Submit the new page as a PR to the MoveIt2 Tutorials repository (https://github.com/ros-planning/moveit2_tutorials.git).

Further Reading
---------------
- This would be a good place for a link to a "Writing How-To's Tutorial" if it existed.
- Maybe even a link to a discussion on the differences between how-to's, tutorials,  discussions, and references .
