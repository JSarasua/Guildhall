#pragma once
#include <vector>
#include <functional>

typedef unsigned int uint;

template <typename ...ARGS>
class Delegate
{
public:
	//using means typedef with extra features
	//ARGS...->extend the list
	using function_t = std::function<bool( ARGS... )>;
	using c_callback_t = bool (*)(ARGS...);

	struct sub_t // subscription_t
	{
		void const* obj_id		= nullptr;
		void const* func_id		= nullptr;
		function_t callable;

		inline bool operator==( sub_t const& other ) const { return (func_id == other.func_id && obj_id == other.obj_id); }
	};

public:

	//functor
	void operator() ( ARGS const& ...args ) { Invoke( args... ); }

	void Subscribe( c_callback_t const& cb )
	{
		sub_t sub;
		sub.func_id = cb;
		sub.callable = cb;

		Subscribe( sub );
	}

	void Unsubscribe( c_callback_t const& cb )
	{
		sub_t sub;
		sub.func_id = cb;

		Unsubscribe( sub );
	}

	template <typename OBJ_TYPE>
	void SubscribeMethod( OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb) (ARGS...) )
	{
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)&mcb; //unsafe if we're using the function with this, but we're justing it as an id

											//lambda is required here because we need to call the objs method aka obj->mcb()
											//[=] captures what's to the right
		sub.callable = [=]( ARGS ...args ) -> bool { return (obj->*mcb)(args...); };

		Subscribe( sub );
	}

	template <typename OBJ_TYPE>
	void UnsubscribeMethod( OBJ_TYPE* obj, bool (OBJ_TYPE::* mcb)(ARGS...) )
	{
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)&mcb; //unsafe if we're using the function with this, but we're justing it as an id

		Unsubscribe( sub );
	}

	void Invoke( ARGS const& ...args )
	{
		for( sub_t& sub : m_subscriptions )
		{
			if( sub.callable( args... ) )
			{
				return;
			}
		}
	}


private:
	void Subscribe( sub_t const& sub )
	{
		m_subscriptions.push_back( sub );
	}

	void Unsubscribe( sub_t const& sub )
	{
		for( uint i = 0; i < m_subscriptions.size(); i++ )
		{
			if( m_subscriptions[i] == sub )
			{
				m_subscriptions.erase( m_subscriptions.begin() + i );
				return;
			}
		}
	}

	void UnsubscribeObject( sub_t const& sub )
	{
		for( uint i = 0; i < m_subscriptions.size(); i++ )
		{
			if( nullptr != m_subscriptions[i] )
			{
				if( m_subscriptions[i].obj_id == sub.obj_id )
				{
					m_subscriptions.erase( m_subscriptions.begin() + i );
					return;
				}
			}
		}
	}

private:
	std::vector<sub_t> m_subscriptions;
};


/*
TEST CODE

class TestClass
{
public:
TestClass( int v )
: m_value( v )
{}

void SomeMethod( int a, int b )
{
int c = a + b + m_value;
DebuggerPrintf( "Method: %i + %i + %i = %i", a, b, m_value, c );
}

private:
int m_value = 0;
};

void App::TestCode()
{

Delegate<float> update;
Delegate<int, int> add;

TestClass obj( 7 );

update.Subscribe( SubscribeTest );
update.Unsubscribe( SubscribeTest );
update.Subscribe( SubscribeTest );
add.Subscribe( AddTest );
add.SubscribeMethod( &obj,  &TestClass::SomeMethod );
add.UnsubscribeMethod( &obj, &TestClass::SomeMethod );
add.SubscribeMethod( &obj, &TestClass::SomeMethod );

//same as
// 	update.Invoke( 3.14f );
// 	add.Invoke( 4, 6 );
update( 3.14f );
add( 4, 6 );




return;
}
*/