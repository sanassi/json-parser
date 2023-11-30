import subprocess
import json
import termcolor

from difflib import unified_diff
from os import listdir
from os.path import isfile, join

from timeout import timeout

import pytest

@timeout(20)
def run_shell(*cmd_line) -> subprocess.CompletedProcess:
    return subprocess.run(cmd_line, capture_output=True, text=True)


def perform_checks(expected: subprocess.CompletedProcess, actual: subprocess.CompletedProcess):
    assert (not expected.stderr) or actual.stderr != "", \
        "Something was expected on stderr"

    assert expected.returncode == actual.returncode, \
        f"Exited with {actual.returncode} expected {expected.returncode}"

    assert expected.stdout == actual.stdout, \
        f"stdout differ\n{diff(expected.stdout, actual.stdout)}"
