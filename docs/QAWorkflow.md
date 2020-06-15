# QA Workflow

This document will explain in detail the Quality Assurance (QA) process which will be used in throughout the development of this project.

## Index


# Tools used
- **Github Issue:** For bug reporting

![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/QA_Workflow/GitHub_Issues.PNG)

To report a bug with an `issue`, the bug reporting template that will be shown at the **Bug Reporting** section must be followed. 


# Milestones
## External Milestones
- **April 14th** Vertical Slice QA
- **April 19th:** Vertical Slice Delivery

- **May 12th** Alpha QA
- **May 17th** Alpha Delivery

- **June 2th** Gold QA
- **June 7th** Gold Delivery

## Internal Milestones
There will be a 1h long internal QA session per sprint, every Saturday. Should there be no new build, the QA session would be either postponed for a day or cancelled.

External QA sessions will be scheduled after a major milestone has been reached.

# Workflow Graphics
- Playtest workflow

- {Pre-Production Workflow}
- {Alpha Workflow}
- {Beta Workflow}
- {Workflow Diagram}

# Bug Reporting
Whenever a team member finds a bug, it must be reported with a Github Issue and following the bug report template.

## Template Parameters
- The bug report template will be the following:

![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/QA_Workflow/Bug_Reporting_Template.PNG)

- [Bug Report Template](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Bug_Reporting_Template.md)


- **Title:** Brief description of the bug.

- **Bug Type:**
```
- Fatal: Gamebreaking, makes the game unplayable.
- Unintended Action: Something is not working as intended.
- Aesthetic: Audio/Visual asset is not being shown as intended.
```

- **Priority:**
```
- ASAP: The bug must be fixed as soon as possible.
- High: The bug must be fixed within 2 days.
- Medium: The bug must be fixed within a week.
- Low: The bug should be fixed whenever there are no higher priority bugs left.
```

- **Frequency:**
```
- Always: Unavoidable during playtime.
- High: Avoidable, but occurs more often than not.
- Average: Happens about 50% of the times.
- Low: Rarely occurs during playtime.
```

- **Description:** Detailed description of the bug. What and Where.

- **Reproduction:** Step by step explanation on how to reproduce the bug. The steps could be uknown.

- **Expected behaviour:** Detailed description on what should happen.

- **Game Version:** The current version of the build. 

- **Additional Context:** Extra information that might be needed to fix the bug.

- **Suggested Assignees:** Suggestion on which team member should be assigned to fix the bug.


**Bug Report Example:**
```
Units pathfind erratically.

- Bug Type: Unintended Action.

- Priority: High.

- Frequency: High.

- Description: Unit paths erratically through the map.

- Reproduction: Left-Click on a unit and then right-click on an inaccessible tile.

- Expected behaviour: Unit should stay idle if the given destination is not accessible.

- Game Version: v0.4 

- Additional Context: Multiple screenshots of the bug reproduction process.

- Suggested Assignees: Code Lead
```

## Other Parameters
On top of using the template shown above, the development team will also use one of the features already integrated in GitHub Issues, the labels:

![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/QA_Workflow/Other_Issue_Parameters.PNG)

### Labels
Will be used to add visual information regarding the current state of each bug and whether or not fixing them is a priority.

#### Label Types
- **Label nºX:**
- **Label nºX:**
- **Label nºX:**
- **Label nºX:**
- **Label nºX:**
- **Label nºX:**
- **Label nºX:**
- **Label nºX:**

{Add a PNG of all the labels}

## QA sessions

### Weekly QA:

- Every Saturday if it's not a week before before a Milestone the team will perform a QA test. This Weekly QA does not need the whole team testing it necessarily though it will be preferred. 

- In this test the team will try to push the boundaries of the game to try and find as many bugs as posible to be fixed, then each one will be submitted to Github Issues. This test will take about 30 minutes or so depending on the time available to the team.

- It would be preferred if right after this testing the team takes some time to fix some of the minor/easily fixed bugs found right away to avoid cluttering the github issues page.

### Milestone QA

- 5 days before each milestone delivery an exhaustive QA will be performed by the members of the team to try and find as many bugs as possible. The game will also be playtested by possible relatives and friends to try and find even more design or technical mistakes. This time the whole team should be present and contributing to the playtest.

- Like in the Weekly QA every bug will be submitted to the github issues page.

### External QA

-  As mentioned before in the Milestone QA there will be external playtesters of our game if possible. This playtest session will be more focused towards making the game more fun and enjoyable for players.

- Since this game is meant to be accessible the testers will have different profiles but some will have to be people who doesn't play videogames often or ever.

This playtest will follow the following steps:

1. Pre-Playtest questions:

- Do you usually use a computer? For what purpose?

- Do you play videogames? If so, how often? In which platform?

- Do you know what an RTS is? If so, have you ever played one?

2. Playtest:

- The tester will set up the game session for the testee and should let it try and go through the whole game. Optionally some tasks can be given to accomplish or certain actions can be told to be performed throughout the session.

- The tester should not give instructions to the testee and should write down observations regarding what parts of the game that is giving them problems as well as parts that the testee liked.

3. Post-Playtest questions:

- Which was the objective of the game?

- Did you feel frustrated at any moment while playing the game?

- Was it intuitive to start playing?

- Was anything confusing?

- Was there anything that you did not like in particular about the game?

- This questions may vary depending on what was tested.

## QA Workflow/Plan References:
- [Final QA Workflow/Plan Reference I](https://github.com/DevCrumbs/Warcraft-II/wiki/8.-QA-Plan)
- [Final QA Workflow/Plan Reference II](https://github.com/CheckTheDog/Fantasy-Brawl/wiki/7.-QA-Plan)
