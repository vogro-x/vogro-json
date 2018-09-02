#ifndef _JSONWRITER_H
#define _JSONWRITER_H

#include <iostream>
#include <string>
#include <stack>
#include <cstdint>
#include <string>

class JsonWriter
{
public:

	enum ContainerType {
		CONTAINER_TYPE_ARRAY,
		CONTAINER_TYPE_OBJECT
	};

	enum ContainerLayout {
		CONTAINER_LAYOUT_INHERIT,
		CONTAINER_LAYOUT_MULTI_LINE,
		CONTAINER_LAYOUT_SINGLE_LINE
	};

	explicit JsonWriter () :      
		writer (NULL),
		initialIndentDepth (0),
		indent ("  "),
		containerPadding (" "),
		keyPaddingLeft (""),
		keyPaddingRight (" "),
		defaultContainerLayout (CONTAINER_LAYOUT_MULTI_LINE),
		forceDefaultContainerLayout (false)
	{
	}

	void ConfigureCompressedOutput ()
	{
		SetIndent ("");
		SetContainerPadding ("");
		SetKeyPaddingLeft ("");
		SetKeyPaddingRight ("");
		SetDefaultContainerLayout (CONTAINER_LAYOUT_SINGLE_LINE);
		SetForceDefaultContainerLayout (true);
	}

	std::ostream *GetWriter () { return writer; }
	void SetWriter (std::ostream *writer) { this->writer = writer; }

	int GetInitialIndentDepth () { return initialIndentDepth; }
	void SetInitialIndentDepth (int depth) { initialIndentDepth = depth; }

	const char *GetIndent () { return indent; }
	void SetIndent (const char *indent) { this->indent = indent; }

	const char *GetContainerPadding () { return containerPadding; }
	void SetContainerPadding (const char *padding) { containerPadding = padding; }

	const char *GetKeyPaddingLeft () { return keyPaddingLeft; }
	void SetKeyPaddingLeft (const char *padding) { keyPaddingLeft = padding; }

	const char *GetKeyPaddingRight () { return keyPaddingRight; }
	void SetKeyPaddingRight (const char *padding) { keyPaddingRight = padding; }

	ContainerLayout GetDefaultContainerLayout () { return defaultContainerLayout; }
	void SetDefaultContainerLayout (ContainerLayout layout) { defaultContainerLayout = layout; }

	bool GetForceDefaultContainerLayout () { return forceDefaultContainerLayout; }
	void SetForceDefaultContainerLayout (bool force) { forceDefaultContainerLayout = force; }

	std::ostream& Write ()
	{
		if (writer == NULL)
			return std::cout;
		return *writer;
	}

	void WriteEscapedChar (char c)
	{
		switch (c) {
		case '"': Write () << "\\\""; break;
		case '\\': Write () << "\\\\"; break;
		case '\b': Write () << "\\b"; break;
		case '\f': Write () << "\\f"; break;
		case '\n': Write () << "\\n"; break;
		case '\r': Write () << "\\r"; break;
		case '\t': Write () << "\\t"; break;
		default: Write () << c; break;
		}
	}

	void WriteString (const char *str)
	{
		Write () << "\"";
		for (int i = 0; str [i] != 0; i++)
			WriteEscapedChar (str [i]);
		Write () << "\"";
	}
	
	void StartChild (bool isKey)
	{
		if (depth.size () == 0) {
			if (initialIndentDepth > 0)
				Indent ();
			return;
		}

		Container *container = depth.top ();
		if (container->childCount > 0 && (
			container->type == CONTAINER_TYPE_ARRAY ||
			(container->type == CONTAINER_TYPE_OBJECT && !container->isKey))) {
			Write () << ",";
			if (container->layout == CONTAINER_LAYOUT_SINGLE_LINE) {
				Write () << containerPadding;
			} else {
				Write () << std::endl;
				Indent ();
			}
		} else if (container->childCount == 0) {
			Write () << containerPadding;
			if (container->layout == CONTAINER_LAYOUT_MULTI_LINE) {
				Write () << std::endl;
				Indent ();
			}
		}

		container->isKey = isKey;
		container->childCount++;
	}

	void StartChild () { StartChild (false); }

	void StartContainer (ContainerType type, ContainerLayout layout)
	{
		if (forceDefaultContainerLayout) {
			layout = defaultContainerLayout;
		} else if (layout == CONTAINER_LAYOUT_INHERIT) {
			if (depth.size () > 0)
				layout = depth.top ()->layout;
			else
				layout = defaultContainerLayout;
		}

		StartChild ();
		depth.push (new Container (type, layout));
		Write () << (type == CONTAINER_TYPE_OBJECT ? '{' : '[');
	}

	void EndContainer ()
	{
		Container *container = depth.top ();
		depth.pop ();

		if (container->childCount > 0) {
			if (container->layout == CONTAINER_LAYOUT_MULTI_LINE) {
				Write () << std::endl;
				Indent ();
			} else {
				Write () << containerPadding;
			}
		}

		Write () << (container->type == CONTAINER_TYPE_OBJECT ? '}' : ']');

		delete container;
	}


	void StartArray () { StartContainer (CONTAINER_TYPE_ARRAY, CONTAINER_LAYOUT_INHERIT); }
	void StartArray (ContainerLayout layout) { StartContainer (CONTAINER_TYPE_ARRAY, layout); }
	void StartShortArray () { StartContainer (CONTAINER_TYPE_ARRAY, CONTAINER_LAYOUT_SINGLE_LINE); }
	void EndArray () { EndContainer (); }

	void StartObject () { StartContainer (CONTAINER_TYPE_OBJECT, CONTAINER_LAYOUT_INHERIT); }
	void StartObject (ContainerLayout layout) { StartContainer (CONTAINER_TYPE_OBJECT, layout); }
	void StartShortObject () { StartContainer (CONTAINER_TYPE_OBJECT, CONTAINER_LAYOUT_SINGLE_LINE); }
	void EndObject () { EndContainer (); }

	void Key (const char *key) {
		StartChild (true);
		WriteString (key);
		Write () << keyPaddingLeft << ":" << keyPaddingRight;
	}

	void NullValue ()
	{
		StartChild ();
		Write () << "null";
	}

	void Value (const char *value)
	{
		StartChild ();
		WriteString (value);
	}

	void Value (std::string value)
	{
		StartChild ();
		WriteString (value.c_str ());
	}

	void Value (bool value)
	{
		StartChild ();
		Write () << (value ? "true" : "false");
	}

	#define VALUE_DEF(t) void Value (t value) { StartChild (); Write () << value; }
	#define KEYVALUE_DEF(t) void KeyValue (const char *key, t value) { Key (key); Value (value); }

	// define a lot of Value functions, which parse number
	VALUE_DEF(int8_t)
	VALUE_DEF(uint8_t)
	VALUE_DEF(int16_t)
	VALUE_DEF(uint16_t)
	VALUE_DEF(int32_t)
	VALUE_DEF(uint32_t)
	VALUE_DEF(int64_t)
	VALUE_DEF(uint64_t)
	VALUE_DEF(float)
	VALUE_DEF(double)

	void KeyNullValue (const char *key) { Key (key); NullValue (); }

	KEYVALUE_DEF(const char *)
	KEYVALUE_DEF(std::string)
	KEYVALUE_DEF(bool)
	KEYVALUE_DEF(int8_t)
	KEYVALUE_DEF(uint8_t)
	KEYVALUE_DEF(int16_t)
	KEYVALUE_DEF(uint16_t)
	KEYVALUE_DEF(int32_t)
	KEYVALUE_DEF(uint32_t)
	KEYVALUE_DEF(int64_t)
	KEYVALUE_DEF(uint64_t)
	KEYVALUE_DEF(float)
	KEYVALUE_DEF(double)

private:

	std::ostream *writer;
	int initialIndentDepth;
	const char *indent;
	const char *containerPadding;
	const char *keyPaddingLeft;
	const char *keyPaddingRight;
	ContainerLayout defaultContainerLayout;
	bool forceDefaultContainerLayout;

	struct Container {
		ContainerType type;
		ContainerLayout layout;
		bool isKey;
		int childCount;

		Container (ContainerType type, ContainerLayout layout) :
			type (type),
			layout (layout),
			isKey (false),
			childCount (0)
		{
		}
	};

	std::stack<Container *> depth;

	void Indent (){
		for (int i = 0, s = initialIndentDepth + depth.size (); i < s; i++)
			Write () << indent;
	}
};

#endif /* _JSONWRITER_H */