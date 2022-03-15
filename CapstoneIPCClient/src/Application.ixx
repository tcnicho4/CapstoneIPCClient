module;

export module Capstone.Application;

export namespace Capstone
{
	class Application
	{
	private:
		Application() = default;

	public:
		Application(const Application& rhs) = delete;
		Application& operator=(const Application& rhs) = delete;

		Application(Application&& rhs) noexcept = default;
		Application& operator=(Application&& rhs) noexcept = default;

		void Run();

	private:
		void Initialize();
		void RunIMPL();

	public:
		static Application& GetInstance();
	};
}