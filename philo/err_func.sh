check_cpu_usage() {
    local program_name="$1"
    local program_path="$2"
    local params=("${@:3:5}")
    local test_number="$7"
    local pid=0
    local start_time=$(date +%s)
    local end_time=$((start_time + 10))
    local current_time=0
	local cpu_usage_sum=0
	local cpu_usage_count=0
	local truncated_result=0
	local pgid=0

    timeout 15 "$program_path/$program_name" "${params[@]}" &>.debug.txt & pid=$!

	pgid=$(ps -o pgid=$pid | grep -o '[1-9]*')
	sleep 1
    while [ $current_time -lt $end_time ]; do
        current_time=$(date +%s)
        cpu_usage=$(ps -C "$program_name" -o %cpu | awk 'NR>1 {sum+=$1} END {printf "%.2f\n", sum}')
        echo "${yellow} CPU usage: $cpu_usage% ${reset}"
		if [ $(echo "$cpu_usage > 50" | bc -l) -eq 1 ]; then
			echo -e "${red}[-] Test #${test_number} Failed: CPU usage is too high ($cpu_usage%) with $3 philos${reset} \n"
			kill &pid

			return 1
		fi
		cpu_usage_sum="$(echo "$cpu_usage_sum + $cpu_usage" | bc -l)"
		cpu_usage_count=$((cpu_usage_count + 1))
        sleep 1
    done

	average_cpu_usage="$(echo "$cpu_usage_sum / $cpu_usage_count" | bc -l)"
	truncated_result=$(printf "%.2f%%\n" $average_cpu_usage)
	echo -e "\n${green}[+] Test #${test_number} Succeeded: Average CPU usage is $truncated_result for $3 philos${reset} \n"
    kill $pid
	rm .debug.txt
}
