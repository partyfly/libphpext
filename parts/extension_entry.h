#pragma once

namespace php {
	class extension_entry {
	public:
		static extension_entry* self;
		extension_entry();
		~extension_entry();
		void init(const char* name, const char* version);
		void done();
		extension_entry& add(const ini_entry& entry);
		extension_entry& add(const constant_entry& entry);
		// 函数
		template<value FUNCTION(parameters& params)>
		extension_entry& add(const char* name) {
			zend_function_entry entry;
			function_entry<FUNCTION>::fill(&entry, name);
			function_entries_.push_back(entry);
		}
		template<value FUNCTION(parameters& params)>
		extension_entry& add(const char* name, arguments&& info) {
			zend_function_entry entry;
			arguments_.emplace_back(std::move(info));
			function_entry<FUNCTION>::fill(&entry, name, arguments_.back());
			function_entries_.push_back(entry);
		}
		// 类
		template <class T>
		extension_entry& add(class_entry<T>&& entry) {
			class_entries_.emplace_back( new class_entry<T>(std::move(entry)) );
		}

		int module;

		zend_module_entry* entry();

		void on_module_startup(std::function<bool (extension_entry&)> handler);
		void on_module_shutdown(std::function<bool (extension_entry&)> handler);
		void on_request_startup(std::function<bool (extension_entry&)> handler);
		void on_request_shutdown(std::function<bool (extension_entry&)> handler);
	private:
		std::vector<ini_entry>      ini_entries_;
		std::vector<constant_entry> constant_entries_;
		std::vector<zend_function_entry> function_entries_;
		std::vector<std::shared_ptr<class_entry_base>> class_entries_;
		std::vector<arguments> arguments_;
		std::list<std::function<bool(extension_entry&)>> handler_rsd_;
		std::list<std::function<bool(extension_entry&)>> handler_rst_;
		std::list<std::function<bool(extension_entry&)>> handler_mst_;
		std::list<std::function<bool(extension_entry&)>> handler_msd_;
		std::vector<zend_module_dep> deps_;

		// 扩展回调函数
		static int on_module_startup_handler  (int type, int module);
		static int on_module_shutdown_handler (int type, int module);
		static int on_request_startup_handler (int type, int module);
		static int on_request_shutdown_handler(int type, int module);


		zend_module_entry entry_;
	};
}
