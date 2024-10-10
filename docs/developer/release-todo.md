# Release Checklist

## Prerelease Source Checking

* make sure the [github actions CI](https://github.com/KDAB/KDReports/actions) is green
* make sure the [KDAB CI](https://kdab.ci.kdab.com/#/projects/25) is green
* look for any static checking issues (clazy, clang-tidy, cppcheck, etc) that may need fixing.
  check [KDAB CI](https://kdab-artifacts.ci.kdab.com/analysis/kdreports-stable)
  and [Github Nightly CI](https://github.com/KDAB/KDReports/actions/workflows/nightly.yml)
* look for any changes in the README.md or INSTALL.txt that may need updating or improving.
* follow the [prerelease source code review](prerelease-source.md) instructions.
* merge any changes made during the review
* create the tag (**do not push the tag yet**)

## Prerelease Build Checking

Any last changes before pushing the tag? Any CI failures?

If so:

```shell
    git -d kdreports-X.Y.Z # delete the tag; good thing we didn't push it yet
    commit and push fixes
    update CHANGES_X_Z.txt as necessary
    ./deploy/release-kdreports.sh X.Y.Z
    git tag -m "KDReports X.Y.Z" kdreports-X.Y.Z
```

## Release

### Push the tag

```shell
git push --tags
```

### Github

#### Make a release on Github

Go to <https://github.com/KDAB/KDReports/releases> and make an official release.

* push the "Draft a new release" button.
* use the CHANGES_X_Y.txt to help write the release description.
* hang the kdreports-X.Y.Z.tar.gz, kdreports-X.Y.Z.zip and kdreports-X.Y.0-doc.zip
  (for major and minor releases) on the github release page.
* also hang the associated .asc files on the github release page.

#### Change the default branch

For major (X.Y.0) releases, change the default branch at
<https://github.com/KDAB/KDReports/settings> to kdreports-X.Y

## Postrelease

### Announcing

* update the
  [Products Release Schedule wiki](https://wiki.kdab.com/display/Products/Product+Release+Schedule)
  and [KDReports wiki](https://wiki.kdab.com/display/Products/KDReports)
  with new version numbers

* email the marketing team <marketing@kdab.com> and ask to have the news of
  the KDReports release posted to KDAB social media.

### Prepare for Next Release

In the branch:

* increment the ${PROJECT_NAME}_VERSION_PATCH value in the top-level CMakeLists.txt
* merge to the "master" branch
