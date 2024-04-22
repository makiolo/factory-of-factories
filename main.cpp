#include <iostream>
#include <utility>
#include "factory.h"

#include "boost/di.hpp"
namespace di = boost::di;


// Parameters: String, Numeric, boolean


auto ValueProduct = []{};
auto ValueExtConditions = []{};


// factory
class ExternalConditions
{
public:
    using factory = dp14::factory<ExternalConditions, int>;

    DEFINE_KEY(ExternalConditions)
    BOOST_DI_INJECT(ExternalConditions, (named = ValueExtConditions) int value)
        : _value(value)
    {
        ;
    }

    int getValue() const {
        return _value;
    }

private:
    int _value;
};

// factory
class Product
{
public:
    using factory = dp14::factory<Product, int, ExternalConditions>;

    DEFINE_KEY(Product)
    BOOST_DI_INJECT(Product,    (named = ValueProduct) int value,
                                                       ExternalConditions ec)
        : _value(value), _ec(std::move(ec))
    {
        ;
    }

    int getValue() const
    {
        return _value;
    }

    int getExternalConditionsValue() const
    {
        return _ec.getValue();
    }

private:
    ExternalConditions _ec;
    int _value;
};

namespace regA
{
    ExternalConditions::factory::registrator<ExternalConditions> reg;
}
namespace regB
{
    Product::factory::registrator<Product> reg;
}

int main()
{
    // ExternalConditions no depends
    auto condition1 = ExternalConditions::factory::instance().create("ExternalConditions", 11);
    // Product depends on ExternalConditions
    auto product1 = Product::factory::instance().create("Product", 22, std::move(*condition1));

    std::cout << "Product value: " << product1->getValue() << std::endl;
    std::cout << "Condition value: " << product1->getExternalConditionsValue() << std::endl;

    const auto injector = di::make_injector(
        di::bind<int>.named(ValueProduct).to(111),
        di::bind<int>.named(ValueExtConditions).to(222)
    );

    auto product2 = injector.create<std::unique_ptr<Product> >();

    std::cout << "Product2 value: " << product2->getValue() << std::endl;
    std::cout << "Condition2 value: " << product2->getExternalConditionsValue() << std::endl;

    return 0;
}
