/*
 * Under MIT license
 * Author: Amir Mohamadi (@amiremohamadi)
 * DuckX is a free library to work wirh docx files.
*/

#ifndef DUCKX_H
#define DUCKX_H

#include <stdlib.h>
#include <cstdio>
#include <string>

#include "pugixml.hpp"
#include "zip.h"


// TODO: Use conatiner-iterator design pattern!

namespace duckx {
  namespace detail {
    template <typename Derived>
    class iterator_facade {
        Derived& derived() noexcept {
          return static_cast<Derived&>(*this);
        }
        const Derived& derived() const noexcept {
          return static_cast<Derived&>(*this);
        }
    public:
        struct sentinel_t{} sentinel;
        Derived& operator*() {
          return derived();
        }
        Derived& operator++() {
          derived().next();
          return derived();
        }
        Derived operator++(int) {
          auto tmp(*this);
          ++(*this);
          return tmp;
        }
        Derived& begin() noexcept {
          return derived();
        }
        const Derived& begin() const noexcept {
          return derived();
        }
        sentinel_t end() noexcept {
          return {};
        }
        sentinel_t end() const noexcept {
          return {};
        }
        friend bool operator==(Derived& d, sentinel_t s) noexcept {
          return !d.has_next(); // has_next() should be const
        }
        friend bool operator!=(Derived& d, sentinel_t s) noexcept {
          return d.has_next();
        }
    };
    }

    // Run contains runs in a paragraph
    class Run : public detail::iterator_facade<Run> {
        friend class detail::iterator_facade<Run>;
    private:
        // Store the parent node (a paragraph)
        pugi::xml_node parent;
        // And store current node also
        pugi::xml_node current;

    public:
        Run();
        Run(pugi::xml_node, pugi::xml_node);
        void set_parent(pugi::xml_node);
        void set_current(pugi::xml_node);

        std::string get_text();
        bool set_text(std::string);
        bool set_text(const char *);

        Run &next();
        bool has_next();
    };

    // Paragraph contains a paragraph
    // and stores runs
    class Paragraph : public detail::iterator_facade<Paragraph> {
        friend class detail::iterator_facade<Paragraph>;
    private:
        // Store parent node (usually the body node)
        pugi::xml_node parent;
        // And store current node also
        pugi::xml_node current;
        // A paragraph consists of runs
        Run run;

    public:
        Paragraph();
        Paragraph(pugi::xml_node, pugi::xml_node);
        void set_parent(pugi::xml_node);
        void set_current(pugi::xml_node);

        Paragraph &next();
        bool has_next();

        Run &runs();
        Run &add_run(std::string);
        Run &add_run(const char*);
        Paragraph &insert_paragraph_after(std::string);
    };

	// TableCell contains one or more paragraphs
	class TableCell : public detail::iterator_facade<TableCell> {
	    friend class detail::iterator_facade<TableCell>;
	private:
		pugi::xml_node parent;
		pugi::xml_node current;

		Paragraph paragraph;
	public:
		TableCell();
		TableCell(pugi::xml_node, pugi::xml_node);

		void set_parent(pugi::xml_node);
		void set_current(pugi::xml_node);

		Paragraph& paragraphs();

		TableCell& next();
		bool has_next();
	};

	// TableRow consists of one or more TableCells
	class TableRow : public detail::iterator_facade<TableRow> {
	    friend class detail::iterator_facade<TableRow>;
		pugi::xml_node parent;
		pugi::xml_node current;

		TableCell cell;
	public:
		TableRow();
		TableRow(pugi::xml_node, pugi::xml_node);
		void set_parent(pugi::xml_node);
		void set_current(pugi::xml_node);

		TableCell& cells();

		bool has_next();
		TableRow& next();
	};

	// Table consists of one or more TableRow objects
	class Table : public detail::iterator_facade<Table> {
	    friend class detail::iterator_facade<Table>;
	private:
		pugi::xml_node parent;
		pugi::xml_node current;

		TableRow row;
	public:
		Table();
		Table(pugi::xml_node, pugi::xml_node);
		void set_parent(pugi::xml_node);
		void set_current(pugi::xml_node);

		Table& next();
		bool has_next();

		TableRow& rows();
	};

    // Document conatins whole the docx file
    // and stores paragraphs
    class Document {
    private:
        std::string directory;
        Paragraph paragraph;
		Table table;
        pugi::xml_document document;

    public:
        Document();
        Document(std::string);
        void file(std::string);
        void open();
        void save();

        Paragraph &paragraphs();
		Table& tables();
    };
}

#endif
