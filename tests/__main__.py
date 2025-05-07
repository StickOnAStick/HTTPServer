import json
import requests
import shutil
import os
import time

from dataclasses import dataclass
from enum import Enum
from loguru import logger
from pathlib import Path
from collections import deque


class SingletonMeta(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]


class TestConfig(metaclass=SingletonMeta):
    iter: int # Number of iterations to run
    test_file: Path
    output_file: Path

    def __init__(self) -> None:
        return
   

@dataclass
class TestOutput:
    response_code: int
    response_time: float
    method: str
    endpoint: str

    @staticmethod
    def headers()->str:
        return f"method, endpoint, response_code, response_time, time_scale\n"
    
    def __repr__(self):
        return f"{self.method} {self.endpoint} {self.response_code} {self.response_time} us\n"

    


def main():

    with open("./config.json", "r") as config_file:
        data = json.load(config_file)
        config.iter = data["iter"]
        config.test_file = Path(data["test_file"])
        config.output_file = Path(data["output_file"])

    test_results: deque[TestOutput] = []

    # Run test file N times
    for _ in range(config.iter):
        # Read each line of the test file
        with open(config.test_file, "r") as test_file:
            for test_case in test_file:
                test_case = test_case.rstrip()  # Remove trailing newlines but keep spaces between words

                if not test_case:
                    continue  # Skip empty lines

                try:
                    method, url = test_case.split(" ", 1)  # Ensure we only split at the first space
                    url = url.strip()  # Strip any accidental surrounding spaces from the URL
                except ValueError:
                    logger.debug(f"Skipping malformed test case: {test_case}")
                    continue
               
                match method:
                    case "GET":
                        res = requests.get(url)
                    case "POST":
                        res = requests.post(url)
                    case "PUT":
                        pass # Not implemented
                    case "DELETE":
                        res = requests.delete(url)
                    case "PATCH":   
                        res = requests.patch(url)
                
                test_out: TestOutput = TestOutput(
                    endpoint=url,
                    method=method,
                    response_code=res.status_code,
                    response_time=res.elapsed.microseconds
                )
                test_results.append(test_out)
                test_case = test_file.readline()
        
        
        # Save the results to output file
        with open(config.output_file, "w+") as output_file:
            output_file.write(TestOutput.headers())
            output_file.writelines([repr(test_out) for test_out in test_results])

def count_lines(filepath):
    """ Counts the numbers of lines in a file"""

    try:
        with open('./scripts/basic.txt', 'r') as file:
            line_count = sum(1 for line in file)
        return line_count
    except FileNotFoundError:
        print(f"Error File not found: {filepath}")
        return None
    except Exception as e:
        print(f"Unknown exception when counting lines: {e}")
        return None

def copy_config():
    try:
        shutil.copy("./config.json", "./output")
        print(f"Config copied over successfully")
    except FileNotFoundError:
        print(f"File: './config.json' not found")
    except PermissionError:
        print("You lack permissions to copy the config file")
    except Exception as e:
        print(f"Unknown exception: {e}")

if __name__ == "__main__":
    logger.info("Beginning test Suite...")
    config = TestConfig()
    start = time.time_ns()
    main()
    end = time.time_ns()

    # Store the total elapsed time
    elapsed = end - start
    # Calc number of requests:
    request_count = count_lines('./scripts/basic.txt')

    
    # Save the config for later
    copy_config()


    # Add the elapsed time to the config json
    with open("./output/config.json", "r+") as config_file:
        data = json.load(config_file)
    
    data['time_ns'] = elapsed
    data["num_requests"] = request_count * data['iter']
    
    with open('./output/config.json', 'w') as f:
        json.dump(data, f, indent=2)

        

    logger.info("Ending Test Suite...")