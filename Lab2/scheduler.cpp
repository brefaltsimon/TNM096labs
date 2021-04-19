#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <optional>
#include <numeric>

// Pairs <timeslot, classroom>
typedef std::vector<std::pair<size_t, size_t>> Assignment;

class SchedulingProblem
{
public:
    SchedulingProblem(std::vector<std::string> classes, std::vector<std::string> timeslots, std::vector<std::string> classrooms)
        : classes{classes}, timeslots{timeslots}, classrooms{classrooms} {};

    size_t num_classes() const { return classes.size(); }
    size_t num_timeslots() const { return timeslots.size(); }
    size_t num_classrooms() const { return classrooms.size(); }

    size_t get_class_index(const std::string &class_name) const
    {
        auto it = std::find(classes.begin(), classes.end(), class_name);
        return it - classes.begin();
    }

    const std::string &get_class(size_t i) const { return classes[i]; }
    const std::string &get_timeslot(size_t i) const { return timeslots[i]; }
    const std::string &get_classroom(size_t i) const { return classrooms[i]; }

    int count_conflicts(const Assignment &ass) const
    {
        return classroom_conflicts(ass) + same_year_conflicts(ass);
    };

    void print_schedule(const Assignment &ass)
    {
        std::cout << std::setw(20);
        for (auto classroom : classrooms)
        {
            std::cout << classroom << std::setw(15);
        }

        std::cout << '\n'
                  << std::setfill('-') << std::setw(55) << '\n'
                  << std::setfill(' ');

        for (size_t timeslot = 0; timeslot < timeslots.size(); ++timeslot)
        {
            std::cout << timeslots[timeslot] << std::setw(15);
            for (size_t classroom = 0; classroom < classrooms.size(); ++classroom)
            {
                auto found = std::find(ass.begin(), ass.end(), std::make_pair(timeslot, classroom));
                if (found != ass.end())
                    std::cout << classes[found - ass.begin()] << std::setw(15);
                else
                    std::cout << std::setw(15);
            }
            std::cout << '\n';
        }
    }

private:
    // Variables
    std::vector<std::string> classes;
    // Values
    std::vector<std::string> timeslots;
    std::vector<std::string> classrooms;

    int same_year_conflicts(const Assignment &ass) const
    {
        std::unordered_map<std::string, int> map;
        for (size_t i = 0; i < classes.size(); ++i)
        {
            char year = classes[i][2];
            if (year == '5')
                continue;
            map[std::to_string(ass[i].first) + year]++;
        }

        int accumulated_conflicts = 0;
        for (auto &i : map)
        {
            accumulated_conflicts += (i.second - 1);
        }

        return accumulated_conflicts;
    };

    int classroom_conflicts(const Assignment &ass) const
    {
        std::unordered_map<std::string, int> map;
        for (auto &i : ass)
        {
            map[std::to_string(i.first) + std::to_string(i.second)]++;
        }

        int accumulated_conflicts = 0;
        for (auto &i : map)
        {
            accumulated_conflicts += (i.second - 1);
        }

        return accumulated_conflicts;
    }
};

std::optional<Assignment> min_conflicts_solver(const SchedulingProblem &csp, int max_steps)
{
    // Randomized initial assignment
    Assignment current(csp.num_classes());
    std::generate(current.begin(), current.end(),
                  [&csp]() {
                      size_t timeslot = rand() % (csp.num_timeslots());
                      size_t classroom = rand() % (csp.num_classrooms());
                      return std::make_pair(timeslot, classroom);
                  });

    // Cache all possible pairs of timeslots and classrooms
    Assignment possible_values;
    for (size_t timeslot = 0; timeslot < csp.num_timeslots(); ++timeslot)
    {
        for (size_t classroom = 0; classroom < csp.num_classrooms(); ++classroom)
        {
            possible_values.push_back(std::make_pair(timeslot, classroom));
        }
    }

    for (int step = 0; step < max_steps; ++step)
    {
        // Have we reached a solution?
        int current_conflicts = csp.count_conflicts(current);
        if (current_conflicts == 0)
            return std::make_optional(current);

        // Select a variable at random
        size_t variable = rand() % csp.num_classes();

        // Compute the number of conflicts for all values
        std::vector<int> num_conflicts;
        std::transform(possible_values.begin(), possible_values.end(), std::back_inserter(num_conflicts),
                       [current, variable, &csp](auto &value) mutable {
                           current[variable] = value;
                           return csp.count_conflicts(current);
                       });

        // Assign the minimizing value to variable in current
        auto min_it = std::min_element(num_conflicts.begin(), num_conflicts.end());
        size_t min_idx = min_it - num_conflicts.begin();
        current[variable].first = possible_values[min_idx].first;
        current[variable].second = possible_values[min_idx].second;
    }

    return std::nullopt;
}

std::optional<Assignment> brute_force_preference_solver(const SchedulingProblem &csp, int max_steps, int max_attempts)
{
    // Compute a lot of schedules
    std::vector<Assignment> schedules;
    for (int i = 0; i < max_attempts; ++i)
    {
        auto found = min_conflicts_solver(csp, max_steps);
        if (found.has_value())
            schedules.push_back(*found);
    }

    // Check which schedule has the fewest preference conflicts
    std::vector<int> preference_conflicts;
    std::transform(schedules.begin(), schedules.end(),
                   std::back_inserter(preference_conflicts),
                   [&csp](auto &schedule) {
                       int num_conflicts = std::accumulate(
                           schedule.begin(), schedule.end(), 0,
                           [&csp](int acc, auto &val) {
                               auto timeslot = csp.get_timeslot(val.first);
                               // General time preferences
                               if (timeslot == "09:00" || timeslot == "12:00" || timeslot == "16:00")
                               {
                                   return acc + 1;
                               }
                               return acc;
                           });

                       // Preferences for MT5
                       auto mt501 = csp.get_class_index("MT501");
                       auto mt501_time = csp.get_timeslot(schedule[mt501].first);
                       if (mt501_time != "13:00" && mt501_time != "14:00")
                           num_conflicts += 1;

                       auto mt502 = csp.get_class_index("MT502");
                       auto mt502_time = csp.get_timeslot(schedule[mt502].first);
                       if (mt502_time != "13:00" && mt502_time != "14:00")
                           num_conflicts += 1;

                       return num_conflicts;
                   });
    // Find the best one in terms of preferences
    auto min_it = std::min_element(preference_conflicts.begin(), preference_conflicts.end());
    auto index = min_it - preference_conflicts.begin();

    return schedules[index];
}

int main()
{
    // Seed the RNG for new randomizations every time
    srand(time(NULL));

    SchedulingProblem csp{{"MT101", "MT102", "MT103", "MT104", "MT105", "MT106", "MT107",
                           "MT201", "MT202", "MT203", "MT204", "MT205", "MT206",
                           "MT301", "MT302", "MT303", "MT304",
                           "MT401", "MT402", "MT403",
                           "MT501", "MT502"},
                          {"09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00"},
                          {"TP51", "SP34", "K4"}};
    auto ass = brute_force_preference_solver(csp, 100, 100);

    if (ass.has_value())
        csp.print_schedule(*ass);
    else
        std::cout << "FAILURE\n";

    return 0;
}