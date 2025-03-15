import json
import requests

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
                        pass # Not implemented
                    case "PUT":
                        pass # Not implemented
                    case "DELETE":
                        pass # Not implemented
                    case "PATCH":   
                        pass # Not implemented
                
                test_out: TestOutput = TestOutput(
                    endpoint=url,
                    method=method,
                    response_code=res.status_code,
                    response_time=res.elapsed.microseconds
                )
                test_results.append(test_out)
                test_case = test_file.readline()
        
        
        # Save the results to output file
        with open(config.output_file, "w") as output_file:
            output_file.write(TestOutput.headers())
            output_file.writelines([repr(test_out) for test_out in test_results])
            

if __name__ == "__main__":
    logger.info("Beginning test Suite...")
    config = TestConfig()
    main()
    logger.info("Ending Test Suite...")