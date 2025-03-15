## Setup and install

This testing suite uses Poetry for dependency management.

If you don't already have poetry installed, please follow their guide [here](https://python-poetry.org/docs/).

### Activate the virtual enviornment
```bash
poetry shell #From inside HTTPServer/tests/
```
### Install deps
```bash
poetry install
```

### Profit?
You're now all set to run the test suite.

### Adding new packages.
Poetry will resolve all package dependecy conflicts for us. However, be careful as this sometimes changes the python version requirements. Be considerate when altering. 

```bash
poetry add {some_pypi_package}
```
