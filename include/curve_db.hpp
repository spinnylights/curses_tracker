#ifndef k32b81dfe81b4451b57247f7c86d880c
#define k32b81dfe81b4451b57247f7c86d880c

#include "db.hpp"

#include <iostream>

class Curve;

/* DB's destructor finalizes the prepared statements
 *
 * if we're going to use SQLITE_STATIC for the Curve data, the Curve objects
 * need to live longer than DB, and thus longer than CurveDB, because DB's
 * destructor will be run after CurveDB's destructor
 *
 * this will be okay /if/ Curves is a separate class that holds both the Curve
 * objects /and/ the CurveDB; then it can arrange for the CurveDB to be
 * destroyed before the Curve objects
 *
 * alternatively, Curve could hold a shared_ptr to a C-style array, and CurveDB
 * could hold those pointers, ensuring that they live as long as the CurveDB;
 * this would be a little more internally complicated but would give Curve more
 * of an independent life, which might be good for such a central class
 *
 * with the shared_ptr approach, i'm not as sure we would even need a Curves
 * class; with the current approach, it would basically mean the rest of the
 * application would interact with curves via the Curves class and not
 * individual Curve objects; Curves might give out references to its internal
 * Curve objects for temporary use with that scheme
 *
 * shared_ptr:
 *     CurveDB cdb;
 *     Curve c {"c"};
 *     c.transeg(...);
 *     curve_handle c_h = cdb.add(c); // c is passed by const reference
 *                                    // cdb stores shared_ptr to c's data
 *
 *     Curve& c = cdb.get(c_h);
 *
 * hmmm...
 *     CurveDB cdb;
 *     Curve& c = cdb.new("name");
 *     c.transeg(...);
 *     cdb.update(c);
 *
 *     Curve& c = cdb.get("name");
 *     c.name("other name");
 *     c.sinusoids(...);
 *     cdb.update(c);
 *
 * of course, if you're taking references, then you're updating internal data
 * in cdb, and arguably it's bad to require the user to call cdb.update() in
 * that case because if they forget cdb's internal data will get out-of-sync
 * with the database
 *
 * one possibility is to have cdb's member fns like new() and get() take a
 * callback that passes a reference to the Curve and then updates the db
 * afterwards; this is a little more cumbersome than the API sketch above
 * though
 *
 * to some extent, it seems desirable to have the in-memory Curve objects be
 * able to change without immediately having to have these changes written to
 * the database, because that allows us to pick freely when we want to write
 * them; like, the user could play around with the curve and then decide
 * if+when they wanted to save it to disc
 *
 * having the Curve objects stored in CurveDB basically implies that the
 * in-memory and on-disc representations of a curve will always be kept
 * in-sync; that doesn't exactly seem ideal though, because a user might want
 * to experiment with tweaking a curve, play the music a bit, then maybe decide
 * they don't want their tweaks and leave the curve editor without saving them
 * to get back the old curve
 *
 * that would be easy to facilitate if the in-memory curve was not locked to
 * the db; you could just toss away the temporary curve representation without
 * comitting it
 *
 * otherwise, you would have to explicitly save a temp version of the old curve
 * and then restore it in the db if the user decides not to save their changes
 *
 * to some extent i feel like it's hard to know which would be ideal for me as
 * the user without actually trying out the program from that perspective; for
 * now maybe we might as well go without whatever is easiest to code, but try to
 * leave our options open if we want to change things later
 *
 * one thing i should remember is not to worry too prematurely about the cost of
 * creating blank Curve objects in memory and so on; it feels heavy but in
 * practice i kind of doubt that 16384 doubles is such a large amount of data to
 * a modern computer--i should wait until i see what the performance is like in
 * practice to be particularly hesitant about it
 *
 * thinking about ActiveRecord's interface...
 *     * they provide new() to get a new db object without saving it to the db,
 *       and create() to get a new db object that automatically gets a new row
 *       at that time; both of these return a db object
 *     * they let you get a db object (of the same type) through various query
 *       methods like first() or find_by()
 *     * they let you write a db object to the database using /its/ interface,
 *       either with save() to write all its data or update({...}) to modify its
 *       data using the provided hash (both in-memory and in the db); they also
 *       allow you to use the collection object to apply changes to e.g. all
 *       records at once
 *     * they let you wipe a db object from the db using its interface as well,
 *       via destroy(), and also let you delete multiple objects at once from
 *       the db via methods on the collection object like desroy_by({...})
 *
 * doing this kind of thing here would require Curve to store a reference to
 * CurveDB, putting the dependency the other way around (which is maybe not a
 * bad idea)
 *
 * i guess to make things easy in that case, Curves could store a shared_ptr to
 * the CurveDB and pass it to each fresh Curve object on creation; then the user
 * wouldn't need to muck with it
 *
 * in Curves:
 *     std::shared_ptr<CurveDB> cdb; // member
 *
 *     new(name) {
 *         Curve c {name, cdb};
 *         return c;
 *     }
 *
 *     create(name) {
 *         Curve c {name, cdb};
 *         c.save();
 *         return c;
 *     }
 *
 * outside:
 *     Curves cs;
 *     Curve c = cs.new();
 *     c.transeg(...);
 *     c.save(); // talks to the stored cdb shared_ptr; without this call
 *                  nothing happens in the db even when c is destroyed
 *
 * of course, this implies that i should use SQLITE_TRANSIENT for the curve
 * data, because with this scheme Curve objects are short-lived
 *
 * my only concern is that this will end up being slow...but as i said earlier i
 * shouldn't worry about that until i know it's slow, because in practice it may
 * be that the amount of data in a Curve is not all that significant
 */
class CurveDB : public DB {
public:
    static const std::string table_name;
    static const std::string id_col;
    static const std::string name_col;
    static const std::string vals_col;

    CurveDB(std::filesystem::path dbfile)
        : DB(dbfile)
                                   //+ std::to_string(ndx)
                                   //+ ","
                                   //+ c.name()
                                   //+ ","
                                   //+ "?"
                                   //");")}
    {
        auto check_table = prep_stmt("SELECT name FROM sqlite_master "
                                     "WHERE type='table'"
                                     " AND name='"
                                     + table_name
                                     + "';")
                               .step();

        // how to refer to the curves in the db? by name? by id? both? neither?
        //
        // after hours of reading about databases i have come to the following
        // conclusion: we are heading for two different curve relations in this
        // project, a "named curve" or "saved curve" relation stored by name in
        // ~/.config and a "param curve" relation within a specific piece which
        // is stored by id# and can be unnamed or have the same name as another
        // param curve; the idea is that you can copy a "saved curve" into a
        // param curve slot for convenience, at which point it becomes a param
        // curve and loses its ties with its "saved curve" model
        //
        // so, actually, this table is fine for param curves, but should be
        // different for saved curves/global curves/memo curves/reference curves
        // w/e
        //
        // that also indicates that this class might be too narrow, since we're
        // ultimately going to have something more like PieceDB and GlobalDB
        if (!check_table.has_data()) {
            prep_stmt("CREATE TABLE "
                      + table_name
                      + "("
                      + id_col + " INTEGER PRIMARY KEY NOT NULL,"
                      + name_col + " TEXT NOT NULL,"
                      + vals_col + " BLOB"
                      ");")
                .step();
        }
    }

    void emplace(Curve& c, int ndx);

    Curve get(id_t id);
};

#endif
