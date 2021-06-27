#pragma once

#include <ostream>
#include <stdexcept>

/*

Example usage:

    // Start the chain by constructing a document with an ostream.
    JsonDocument doc(std::cout);  // Prints: {

    // Construct a root object; its constructor prints
    // open brace, and its destructor will print closing.
    JsonObject foo(doc["foo"]);  // Prints: "foo":\n    {

    // Scalars are assigned directly to attributes.
    foo["x"] = 1;  // Prints: "x": 1
    foo["y"] = 2;  // Prints: ,"y": 2
    // (Note the leading comma, which was added by the root.)

    // Unlike scalars, to attach an object to an attribute
    // you must use the decorator pattern.
    JsonObject obj1(foo["obj1"]);  // Prints: ,"obj1":{}

    // However typically an object would be printed by
    // a function, and the attribute serve as stream arg.
    print_obj2(foo["obj2"], data.obj2);

    // Array attributes are created similar to objects.
    JsonArray arr1(foo["arr1"]);  // Prints: "arr1": [

    // Arrays don't use attribute names, just values.
    // Any number of values can be passed with parens op.
    arr1(1, 2, 3);  // Prints: 1, 2, 3
    arr1(99);  // Prints: ,99

    // TODO: make user manually close, or autodetect?
    arr1.close();  // prints ]

    // However just as with attributes, printing objects
    // in a JSON array requires using the decorator pattern.
    // Read it inside-out; it prints: ,"arr2": [ {} ]
    JsonObject baz(JsonArray(foo["arr2"]));

    // Once again, there would normally be a call to a
    // user-function involved, which is easier to read.
    JsonArray arr3(foo["arr3"]);
    for (auto elem : data.elements)
        print_element(arr3, elem);
 */

class JsonObject;
class JsonArray;

// Subclasses of this implement decorator pattern.
class JsonPrinter
{
public:
    virtual std::ostream& get_stream() = 0;
    virtual void make_indent(int depth) = 0;
    virtual int depth() const = 0;
    virtual int num_lines() const = 0;

protected:

    template <typename T>
    void print_child(T const& value)
    {
        auto& os = get_stream();

        if constexpr(std::is_invocable_v<T, JsonObject>)
        {
            os << "{";
            int lines_before = num_lines();
            value(*this);
            if (num_lines() > lines_before)
                make_indent(depth());
            os << "}";
        }
        else if constexpr(std::is_invocable_v<T, JsonArray>)
        {
            os << "[";
            int lines_before = num_lines();
            value(*this);
            if (num_lines() > lines_before)
                make_indent(depth());
            os << "]";
        }
        else
        {
            os << "\"" << value << "\"";
        }
    }
};

class JsonPrinterBase : public JsonPrinter
{
protected:
    JsonPrinter& printer;

    JsonPrinterBase(JsonPrinter& printer)
        : printer(printer) {}

public:
    int depth() const override { return printer.depth()+1; }
    std::ostream& get_stream() override { return printer.get_stream(); }
    void make_indent(int depth) override { printer.make_indent(depth); }
    int num_lines() const { return printer.num_lines(); }
};

class JsonAttribute : public JsonPrinterBase
{
public:
    explicit JsonAttribute(JsonPrinter& printer)
        : JsonPrinterBase(printer) {}

    // Attribute heading doesn't increase indent depth
    int depth() const override { return printer.depth(); }

    template <typename T>
    void operator =(T const& value)
    {
        print_child<T>(value);
    }
};

class JsonObject : public JsonPrinterBase
{
private:
    bool needs_comma = false;

public:
    JsonObject(JsonPrinter& printer)
        : JsonPrinterBase(printer) {}

    JsonAttribute operator[](char const* name)
    {
        auto& os = get_stream();

        if (needs_comma)
            os << ",";
        else
            needs_comma = true;

        make_indent(depth());
        os << "\"" << name << "\": ";

        return JsonAttribute(*this);
    }
};

class JsonArray : public JsonPrinterBase
{
private:
    bool needs_comma = false;

public:
    JsonArray(JsonPrinter& printer)
        : JsonPrinterBase(printer) {}

    template <typename T>
    void operator()(T const& value)
    {
        auto& os = get_stream();

        if (needs_comma)
            os << ", ";
        else
            needs_comma = true;

        os << "\"" << value << "\"";
    }

    template <typename T0, typename... Ts>
    void operator()(T0 const& value0, Ts const&... values)
    {
        operator()(value0);
        operator()(values...);
    }

    template <typename T>
    JsonArray& operator +=(T const& value)
    {
        auto& os = get_stream();

        if (needs_comma)
            os << ",";
        else
            needs_comma = true;

        make_indent(depth());

        print_child<T>(value);

        return *this;
    }
};

class JsonDocument : private JsonPrinter
{
private:
    std::ostream& os;
    JsonObject root;
    int lines = 1;

public:
    JsonDocument(std::ostream& os) : os(os), root(*this)
    {
        os << "{";
    }

    void close()
    {
        os << "\n}";
    }

    int depth() const override { return 0; }

    int num_lines() const override { return lines; }

    JsonObject& get_root()
    {
        return root;
    }

    std::ostream& get_stream() override { return os; }

    virtual void make_indent(int depth)
    {
        ++lines;
        os << "\n";
        for (int i=0; i<depth; i++)
                get_stream() << "    ";
    }
};


