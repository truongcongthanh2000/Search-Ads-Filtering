import requests
from concurrent.futures import ThreadPoolExecutor, as_completed
import time

def read_strings_from_file(file_path):
    with open(file_path, 'r') as file:
        return file.read().splitlines()

def send_request(url, query):
    start_time = time.time()
    try:
        response = requests.get(f"{url}{query}")
        end_time = time.time()
        response_json = response.json()
        length = int(response_json.get("length", None))
        return (query, response.status_code, length, start_time, end_time)
    except requests.RequestException as e:
        end_time = time.time()
        return (query, None, str(e), start_time, end_time)

def send_http_requests_concurrently(strings):
    urls = {
        "single_thread": "http://localhost:8080/filter_single_thread/?query=",
        "multi_thread": "http://localhost:8080/filter_multi_thread/?query="
    }
    
    results = {
        "single_thread": [],
        "multi_thread": []
    }
    counter = 0
    report_interval = 1000

    with ThreadPoolExecutor(max_workers=15) as executor:
        future_to_request = {
            executor.submit(send_request, url, string): (key, string)
            for string in strings
            for key, url in urls.items() 
        }

        for future in as_completed(future_to_request):
            key, query = future_to_request[future]
            try:
                data = future.result()
                results[key].append(data)
                counter += 1
                if counter % report_interval == 0:
                    print(f"{counter} requests completed")
                # print(f"Response from {urls[key]} for query '{query}': {data[2]}")
            except Exception as exc:
                print(f"Request to {urls[key]} for query '{query}' generated an exception: {exc}")

    for key in urls.keys():
        total_requests = len(results[key])
        if total_requests > 0:
            start_times = [result[3] for result in results[key]]
            end_times = [result[4] for result in results[key]]
            total_time = 0
            for i in range(len(start_times)):
                total_length += lengths[i]
                total_time += end_times[i] - start_times[i]
            rps = total_requests / total_time if total_time > 0 else 0
            
            print(f"Type: {key}")
            print(f"Total requests: {total_requests}")
            print(f"Total time: {total_time:.2f} seconds")
            print(f"Requests per second (RPS): {rps:.2f}")

if __name__ == "__main__":
    file_path = 'queries.txt'
    strings = read_strings_from_file(file_path)
    send_http_requests_concurrently(strings)
