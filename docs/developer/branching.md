# Branching

Stable branches are branched from the "master" branch and called kdreports-X.Y,
where 'X' the major version number and 'Y' is the minor version number.

We may refer to the "master" branch as the unstable branch.
The currently actively kdreports-X.Y branch is known as the stable branch.

Bugfixes can stay in the current stable branch and do not require a new branch, which
means that the following instructions can be skipped for patch-point releases.

## New minor version

For small feature modifications we need to create a new stable branch.
If the current stable branch is kdreports-X.Y then the new branch will be called kdreports-X.Y+1.

For example, if the current stable branch is 2.1 then the new stable branch will be 2.2, like so:

```shell
    git checkout master
    git branch kdreports-2.2
    git checkout kdreports-2.2
```

## New major version

However, for large feature modifications or API changes we need a major stable release.
If the current stable branch is kdreports-X.Y then the new branch will be called kdreports-X+1.0.

For example, if the current stable branch is 2.1 then the new stable branch will be 3.0, like so:

```shell
    git checkout master
    git branch kdreports-3.0
    git checkout kdreports-3.0
```

## After branching

Once the new branch is created we need to modify a few things.

### Source code changes

For a new major version:

* In the top-level `CMakeLists.txt` set `${PROJECT_NAME}_VERSION_MINOR` to 95
* In the top-level `CMakeLists.txt` set `${PROJECT_NAME}_VERSION_PATCH` to 95
* In the top-level `CMakeLists.txt` set `${PROJECT_NAME}_SOVERSION` to the new X.0
* Add a new `docs/CHANGES_X_Y.txt` ("What's new in KDReports X.Y")

For a new minor version:

* In the top-level `CMakeLists.txt` set `${PROJECT_NAME}_VERSION_PATCH` to 95
* In the a new `docs/CHANGES_X.Y.Z` ( ("What's new in KDReports X.Y.Z")

For any new branch, make sure to add entries for new branch name into the
`.github/workflows/build.yml` file,  inside the 'push' and 'pull_request" branch lists.

Make all the above changes and then git commit and push the new branch.

### And finally

* Update the KDABCI to the new stable branch (contact the KDAB CI team <buiildmaster@kdab.com>)
* Update the Github Default Branch (see Default Branch setting at <https://github.com/KDAB/KDReports/settings>)
* Initiate CI builds (both github and KDAB) of the new branch and make sure all is green before continuing.
