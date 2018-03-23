

#include "factory.h"

Pizza* PizzaStore::OrderPizza(string type)
{
	Pizza* pizza = this->CreatePizza(type);
	if (!pizza)
		return NULL;

	pizza->Prepare();
	pizza->Bake();
	pizza->Cut();
	pizza->Box();
}

Pizza* NYPizzaStore::CreatePizza(string type)
{
	if (type == "cheese")
		return new NYCheesePizza();
	else if(type=="meat")
		return new NYMeatPizza();
	return NULL;

}