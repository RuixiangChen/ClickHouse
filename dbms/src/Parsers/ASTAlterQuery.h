/* Some modifications Copyright (c) 2018 BlackBerry Limited

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#pragma once

#include <Parsers/IAST.h>
#include <Parsers/ASTQueryWithOutput.h>
#include <Parsers/ASTQueryWithOnCluster.h>


namespace DB
{

/** ALTER query:
 *  ALTER TABLE [db.]name_type
 *      ADD COLUMN col_name type [AFTER col_after],
 *      DROP COLUMN col_drop [FROM PARTITION partition],
 *      MODIFY COLUMN col_name type,
 *      ADD TO PARAMETER param_name value,
 *      DROP FROM PARAMETER param_name value,
 *      MODIFY PARAMETER param_name value
 *      DROP PARTITION partition,
 *      RESHARD [COPY] PARTITION partition
 *          TO '/path/to/zookeeper/table' [WEIGHT w], ...
 *              USING expression
 *              [COORDINATE WITH 'coordinator_id']
 * ALTER CHANNEL [dn.]name_type
 *      ADD live_view,...
 *      DROP live_view,...
 *      SUSPEND live_view,...
 *      RESUME live_view,...
 *      REFRESH live_view,...
 *      MODIFY live_view,...
 */

class ASTAlterQuery : public ASTQueryWithOutput, public ASTQueryWithOnCluster
{
public:
    enum ParameterType
    {
        ADD_COLUMN,
        DROP_COLUMN,
        MODIFY_COLUMN,
        MODIFY_PRIMARY_KEY,

        DROP_PARTITION,
        ATTACH_PARTITION,
        FETCH_PARTITION,
        FREEZE_PARTITION,
        RESHARD_PARTITION,

        NO_TYPE,

        ADD_TO_PARAMETER,
        DROP_FROM_PARAMETER,
        MODIFY_PARAMETER,

        CHANNEL_ADD,
        CHANNEL_DROP,
        CHANNEL_SUSPEND,
        CHANNEL_RESUME,
        CHANNEL_REFRESH,
        CHANNEL_MODIFY
    };

    struct Parameters
    {
        Parameters();

        int type = NO_TYPE;

        /** The ADD COLUMN query stores the name and type of the column to add
          *  This field is not used in the DROP query
          *  In MODIFY query, the column name and the new type are stored here
          */
        ASTPtr col_decl;

        /** The ADD COLUMN query here optionally stores the name of the column following AFTER
          * The DROP query stores the column name for deletion here
          */
        ASTPtr column;

        /** For MODIFY PRIMARY KEY
          */
        ASTPtr primary_key;

        /** In DROP PARTITION and RESHARD PARTITION queries, the value or ID of the partition is stored here.
          */
        ASTPtr partition;

        /** In ADD TO PARAMETER, DROP FROM PARAMETER, and MODIFY PARAMETER queries, the value of the parameter name is stored here.
          */
        ASTPtr parameter;

        /** In ADD TO PARAMETER, DROP FROM PARAMETER, and MODIFY PARAMETER queries, the values of the parameter is stored here.
         *  IN ALTER CHANNEL, ADD, DROP, SUSPEND, RESUME, REFRESH, MODIFY queries, the list of live view is tored here
         */
        ASTPtr values;

        bool detach = false;        /// true for DETACH PARTITION

        bool part = false;          /// true for ATTACH PART

        bool do_copy = false;       /// for RESHARD PARTITION

        bool clear_column = false;  /// for CLEAR COLUMN (do not drop column from metadata)

        /** For FETCH PARTITION - the path in ZK to the shard, from which to download the partition.
          */
        String from;

        /** For RESHARD PARTITION.
          */
        ASTPtr weighted_zookeeper_paths;
        ASTPtr sharding_key_expr;
        ASTPtr coordinator;

        /** For FREEZE PARTITION - place local backup to directory with specified name.
          */
        String with_name;

        /// deep copy
        void clone(Parameters & p) const;
    };

    using ParameterContainer = std::vector<Parameters>;
    ParameterContainer parameters;
    String database;
    String table;
    bool is_channel{false}; /// true for ALTER CHANNEL


    void addParameters(const Parameters & params);

    explicit ASTAlterQuery(StringRange range_ = StringRange());

    /** Get the text that identifies this element. */
    String getID() const override;

    ASTPtr clone() const override;

    ASTPtr getRewrittenASTWithoutOnCluster(const std::string & new_database) const override;

protected:
    void formatQueryImpl(const FormatSettings & settings, FormatState & state, FormatStateStacked frame) const override;
};

}
