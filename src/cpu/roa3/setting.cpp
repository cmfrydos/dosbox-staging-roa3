/***********************************************************************
 * Copyright:    (C) 2023 cmfrydos
 * License:      GNU General Public License version 2 or later.
 *
 *               This program is free software; you can redistribute it and/or
 *               modify it under the terms of the GNU General Public License
 *               as published by the Free Software Foundation; either version 2
 *               of the License, or (at your option) any later version.
 *
 *               This program is distributed in the hope that it will be useful,
 *               but WITHOUT ANY WARRANTY; without even the implied warranty of
 *               MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *               GNU General Public License for more details.
 *
 *               You should have received a copy of the GNU General Public
 *License along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***********************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <typeindex>
#include <memory>
#include <optional>

// ToDO: All classes under development

// Custom exceptions
class value_error : public std::runtime_error {
public:
	value_error(const std::string& message)
		: std::runtime_error(message)
	{
	}
};

class dependency_error : public std::runtime_error {
public:
	dependency_error(const std::string& message)
		: std::runtime_error(message)
	{
	}
};

// Forward declarations
template <typename T>
class setting;

// DependencyType enum
enum class dependency_type {
	other_in_range,
	self_in_range,
	other_lambda,
	self_lambda,
	other_value
};

// Dependency class
template <typename T>
class dependency {
private:
	std::string description_;
	dependency_type dependency_type_;
	setting<T>* main_target_;
	setting<T>* other_target_;
	std::pair<T, T> range_;
	std::function<bool(const T&)> lambda_func_;

public:
	dependency(const std::string& desc, dependency_type type,
	           setting<T>* main, setting<T>* other  = nullptr,
	           const std::pair<T, T>& range         = std::pair<T, T>(),
	           std::function<bool(const T&)> lambda = nullptr)
		: description_(desc),
		  dependency_type_(type),
		  main_target_(main),
		  other_target_(other),
		  range_(range),
		  lambda_func_(lambda)
	{
	}

	bool is_met(const T& value) const
	{
		switch (dependency_type_) {
		case dependency_type::other_in_range: if (other_target_) {
				T other_value = other_target_->getValue();
				return range_.first <= other_value &&
				       other_value <= range_.second;
			}
			break;
		case dependency_type::self_in_range: return
				range_.first <= value && value <= range_.second;
		case dependency_type::other_lambda: if (
				other_target_ && lambda_func_) {
				return lambdaFunc(other_target_->getValue());
			}
			break;
		case dependency_type::self_lambda: if (lambda_func_) {
				return lambdaFunc(value);
			}
			break;
		case dependency_type::other_value: if (other_target_) {
				return other_target_->getValue() == value;
			}
			break;
		}
		return false;
	}
};

class setting_base {
protected:
	std::string name_;
	std::string description_;
	bool was_set_;
	std::vector<std::string> search_tags_;
	std::vector<std::string> groups_;

public:
	virtual ~setting_base()
	{
	} // Virtual destructor for proper cleanup

	std::string getName()
	{
		return name_;
	}

	// Add virtual functions here that can be overridden by setting<T>
};

// Setting class
template <typename T>
class setting final : public setting_base {
	T value_;
	T default_value_;
	std::vector<dependency<T>> dependencies_;


	std::vector<std::function<void(const T&, const T&)>> callbacks_;

public:
	setting(const std::string& name, const std::string& description,
	        const T& default_value)
		: name_(name),
		  description_(description),
		  was_set_(false),
		  value_(default_value),
		  default_value_(default_value)
	{
	}

	const std::string& get_name() const
	{
		return name_;
	}

	void validate(const T& new_value)
	{
		for (const auto& dependency : dependencies_) {
			if (!dependency.isMet(new_value)) {
				throw dependency_error(
					"Dependency check failed for setting: "
					+ name_);
			}
		}
	}

	void set_value(const T& new_value)
	{
		validate(new_value);
		T old_value = value_;
		value_      = new_value;
		was_set_    = true;
		for (const auto& callback : callbacks_) {
			callback(old_value, new_value);
		}
	}

	T get_value() const
	{
		return value_;
	}

	void reset_to_default()
	{
		setValue(default_value_);
		was_set_ = false;
	}

	void add_dependency(const dependency<T>& dependency)
	{
		dependencies_.push_back(dependency);
	}

	void add_callback(
		const std::function<void(const T&, const T&)>& callback)
	{
		callbacks_.push_back(callback);
	}

	void add_search_tag(const std::string& tag)
	{
		search_tags_.push_back(tag);
	}

	void add_group(const std::string& group)
	{
		groups_.push_back(group);
	}

private:
	std::vector<std::function<void(setting<T>&)>> dynamic_adjustments_;

public:
	// Add a dynamic adjustment function
	void add_dynamic_adjustment(
		const std::function<void(setting<T>&)>& adjust_func)
	{
		dynamic_adjustments_.push_back(adjust_func);
	}

	// Remove a dynamic adjustment function
	void remove_dynamic_adjustment(
		const std::function<void(setting<T>&)>& adjust_func)
	{
		dynamic_adjustments_.erase(std::remove(
			                           dynamic_adjustments_.begin(),
			                           dynamic_adjustments_.end(),
			                           adjust_func),
		                           dynamic_adjustments_.end());
	}

	// Perform dynamic adjustments
	void perform_dynamic_adjustments()
	{
		for (auto& adjust_func : dynamic_adjustments_) {
			adjust_func(*this);
		}
	}

	// Serialize the setting to a string (JSON-like format)
	std::string serialize() const
	{
		std::ostringstream oss;
		oss << "{ \"name\": \"" << name << "\", \"value\": " << value <<
			" }";
		return oss.str();
	}

	// Deserialize the setting from a string (JSON-like format)
	void deserialize(const std::string& data)
	{
		// Implement JSON parsing to extract name and value
		// Update the setting's value based on the parsed data
		// This is a placeholder for actual JSON parsing logic
	}

	// Check if the setting matches a search query
	bool matches_search(const std::string& query) const
	{
		// Implement search logic based on name, description, and tags
		// This is a placeholder for actual search logic
		return name.find(query) != std::string::npos ||
		       description.find(query) != std::string::npos;
	}

	std::optional<T> temp_value_;

	// Set a temporary value
	void set_value_temporary(const T& new_value)
	{
		temp_value_ = new_value;
	}

	// Validate the temporary value
	bool validate_temporary() const
	{
		return temp_value_.has_value() && validate(temp_value_.value());
	}

	// Commit the temporary value
	void commit_temporary()
	{
		if (!temp_value_.has_value() || !validate_temporary()) {
			throw value_error(
				"Temporary value is invalid or not set.");
		}
		set_value(temp_value_.value());
		temp_value_.reset();
	}

	// Remove a callback function
	void remove_callback(
		const std::function<void(const T&, const T&)>& callback)
	{
		callbacks_.erase(
			std::remove(callbacks_.begin(),
			            callbacks_.end(),
			            callback),
			callbacks_.end());
	}

	// Notify all callbacks of a value change
	void notify_callbacks(const T& old_value, const T& new_value)
	{
		for (const auto& callback : callbacks_) {
			callback(old_value, new_value);
		}
	}
};

// SettingGroup class
class setting_group {
	std::string name_;
	std::map<std::string, std::shared_ptr<setting_base>> settings_;

public:
	setting_group(const std::string& name)
		: name_(name)
	{
	}

	void add_setting(const std::shared_ptr<setting_base>& setting)
	{
		settings_[setting->getName()] = setting;
	}

	template <typename T>
	std::shared_ptr<setting<T>> get_setting(const std::string& setting_name)
	{
		const auto it = settings_.find(setting_name);
		if (it != settings_.end()) {
			return it->second;
		}
		throw std::out_of_range("Setting not found: " + setting_name);
	}

	template <typename T>
	const std::map<std::string, std::shared_ptr<setting<T>>>&
	get_settings() const
	{
		return settings_;
	}
};

// SettingsManager class
class settings_manager {
private:
	std::map<std::type_index, std::map<std::string, std::shared_ptr<void>>>
	groups_;

public:
	template <typename T>
	void add_setting(const std::string& group_name,
	                 const std::shared_ptr<setting<T>>& setting)
	{
		const std::type_index type_index(typeid(T));
		if (groups_.find(type_index) == groups_.end()) {
			groups_[type_index] = std::map<
				std::string, std::shared_ptr<void>>();
		}
		groups_[type_index][group_name] = std::static_pointer_cast<
			void>(setting);
	}

	template <typename T>
	std::shared_ptr<setting<T>> get_setting(const std::string& setting_name)
	{
		const std::type_index type_index(typeid(T));
		const auto it = groups_.find(type_index);
		if (it != groups_.end()) {
			for (const auto& group : it->second) {
				auto setting_group =
					std::static_pointer_cast<
						setting_group < T> > (group.
						second);
				try {
					return setting_group->getSetting(
						setting_name);
				} catch (const std::out_of_range&) {
					// Continue searching in other groups
				}
			}
		}
		throw std::out_of_range("Setting not found: " + setting_name);
	}

	template <typename T>
	void set_setting_value(const std::string& setting_name, const T& value)
	{
		auto setting = get_setting<T>(setting_name);
		setting->setValue(value);
	}

	// Find settings based on a search term
	std::map<std::string, std::vector<std::string>> find_settings(
		const std::string& search_term) const
	{
		std::map<std::string, std::vector<std::string>> matches;
		for (const auto& group_pair : groups_) {
			const auto& group = group_pair.second;
			for (const auto& setting_pair : group) {
				// ToDO
			}
		}
		return matches;
	}
};
