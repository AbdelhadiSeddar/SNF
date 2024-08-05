using System.Linq;
using System.Collections.Generic;
using System.Text;

namespace SNFClient
{
    /// <summary>
    ///     Objects of this class Define the action to do to the host
    /// </summary>
    public partial class OPCode
    {
        private Member category;
        private Member subcategory;
        private Member command;
        private Member detail;

        /// <summary>
        ///     OPCode's Category
        /// </summary>
        public Member Category 
            => category;
        /// <summary>
        ///     OPCode's Subcategory
        /// </summary>
        public Member Subcategory 
            => subcategory;
        /// <summary>
        ///     OPCode's Command
        /// </summary>
        public Member Command 
            => command;
        /// <summary>
        ///     OPCode's Detail
        /// </summary>
        public Member Detail 
            => detail;

        private OPCode() { }

        /// <summary>
        ///     Generates an OPCode Instnce Depending on the Registred Parameters
        /// </summary>
        /// <param name="Category">OPCode's Category</param>
        /// <param name="SubCategory">OPCode's Subcategory</param>
        /// <param name="Command">OPCode's Command</param>
        /// <param name="Detail">OPCode's Detail</param>
        /// <returns>
        ///     OPCode Instance | <c>null</c> if any of the parameters are invalid or not registred
        /// </returns>
        /// <exception cref="Exceptions.ClassUninitialized">
        ///     Thrown When The OPCode.Base Class was not Initialized 
        /// </exception>
        /// <remarks>
        ///     Initialize OPCode.Base with <see cref="OPCode.Base.Initialize()"/> beforehand
        /// </remarks>
        public static OPCode get(byte Category, byte SubCategory, byte Command, byte Detail = 0)
        {
            if(!Base.isInitialized)
                throw new Exceptions.ClassUninitialized("OPCode.Base");

            OPCode op = new OPCode();
            Member mmbr = null;
            if ((mmbr = Member.getCategory(Category)) == null)
                return null;
            op.category = mmbr;

            if ((mmbr = Member.getSubCategory(mmbr, SubCategory)) == null)
                return null;
            op.subcategory = mmbr;

            if ((mmbr = Member.getCommand(mmbr, Command)) == null)
                return null;
            op.command = mmbr;

            if ((mmbr = Member.getDetail(mmbr, Detail)) == null)
                return null;
            op.detail = mmbr;

            return op;
        }
        /// <summary>
        ///     Generates an OPCode Instnce Depending on the Registred Parameters
        /// </summary>
        /// <param name="Category">OPCode's Category</param>
        /// <param name="SubCategory">OPCode's Subcategory</param>
        /// <param name="Command">OPCode's Command</param>
        /// <param name="Detail">OPCode's Detail</param>
        /// <returns>
        ///     OPCode Instance | <c>null</c> if any of the parameters are invalid or not registred
        /// </returns>
        /// <exception cref="Exceptions.ClassUninitialized">
        ///     Thrown When The OPCode.Base Class was not Initialized 
        /// </exception>
        /// <exception cref="Exceptions.OPCode.InvalidRankFound">
        ///     Thrown when you place a Member in a place with different rank
        ///     eg: Put Member with SubCategory Rank in the Category parameter
        /// </exception>
        /// <remarks>
        ///     Initialize OPCode.Base with <see cref="OPCode.Base.Initialize()"/> beforehand
        /// </remarks>
        public static OPCode get(Member Category, Member SubCategory, Member Command, Member Detail = null)
        {
            if (!Base.isInitialized)
                throw new Exceptions.ClassUninitialized("OPCode.Base");
            OPCode op = new OPCode();
            if((op.category = Category).MemberRank != Member.Rank.Category)
                throw new Exceptions.OPCode.InvalidRankFound(Member.Rank.Category, op.category.MemberRank);

            if ((op.subcategory = SubCategory).MemberRank != Member.Rank.SubCategory)
                throw new Exceptions.OPCode.InvalidRankFound(Member.Rank.SubCategory, op.subcategory.MemberRank);

            if ((op.command = Command).MemberRank != Member.Rank.Command)
                throw new Exceptions.OPCode.InvalidRankFound(Member.Rank.Command, op.command.MemberRank);

            if(Detail == null) op.detail = op.command.Sub.First();

            if ((op.detail = Detail).MemberRank != Member.Rank.Detail)
                throw new Exceptions.OPCode.InvalidRankFound(Member.Rank.Detail, op.detail.MemberRank);

            return op;
        }

        /// <summary>
        ///     Turns the OPCode Into a Char of bytes
        /// </summary>
        /// <returns>
        ///     byte[] containing the OPCode sequence
        /// </returns>
        public byte[] ToBytes()
        {
            
            char[] re =
            {
                (char)Category.Value,
                (char)Subcategory.Value,
                (char)Command.Value,
                (char)Detail.Value,
            };
            return Encoding.ASCII.GetBytes(re);
        }
        /// <summary>
        ///     Checks if the OPCode is a <see cref="Base"/> OPCode Member
        /// </summary>
        /// <returns>
        ///     <c>true</c> if is a <see cref="Base"/> OPCode Member else <c>false</c>
        /// </returns>
        public bool isBase() 
            => Category.Value == Base.CATEGORY 
            && Subcategory.Value == Base.SUBCATEGORY;
        /// <summary>
        ///     Compares Between this instance of the OPCode and another Instance
        /// </summary>
        /// <param name="op">OPCode Instance to Compare to</param>
        /// <returns>
        ///     <c>true</c> if They're equal (Excluding Detail Member) else <c>false</c>
        /// </returns>
        /// <remarks>
        /// > [!WARNING]
        /// > This Function ignores the Member with the <see cref="Member.Rank.Detail"/> if you want to compare
        /// > that too, use <see cref="EqualsD(OPCode)"/>
        /// </remarks>
        public bool Equals(OPCode op)
        {
            if(this.Category.Value == op.Category.Value
                && this.Subcategory.Value == op.Subcategory.Value
                && this.Command.Value == op.Command.Value)
                return true;
            return false;
        }
        /// <summary>
        ///     Compares Between this instance of the OPCode and another Instance
        /// </summary>
        /// <param name="op">OPCode Instance to Compare to</param>
        /// <returns>
        ///     <c>true</c> if They're equal else <c>false</c>
        /// </returns>
        public bool EqualsD(OPCode op)
        {
            if (this.Category.Value == op.Category.Value
                && this.Subcategory.Value == op.Subcategory.Value
                && this.Command.Value == op.Command.Value
                && this.Detail.Value == op.Detail.Value)
                return true;
            return false;
        }



        /// <summary>
        ///     Objects of this class Defines every OPCode instance
        /// </summary>
        public class Member
        {
            /// <summary>
            ///     Defines the Ranks of Members
            /// </summary>
            public enum Rank
            {
                /// <summary>
                /// The Category of the OPCode
                /// </summary>
                Category,
                /// <summary>
                /// The Subcategory of the OPCode
                /// </summary>
                SubCategory,
                /// <summary>
                /// The Command of the OPCode
                /// </summary>
                Command,
                /// <summary>
                /// The Detail of the OPCode Command
                /// </summary>
                Detail
            }

            #region Member Static

            private static Member Undefined_Detail 
                = new Member(Rank.Detail, 0x00, "Undefined Detail");

            private static LinkedList<Member> categories 
                = new LinkedList<Member>();
            /// <summary>
            ///     Gets The Saved Categories ( Or the Structure of the Registred OPCodes )
            /// </summary>
            public static LinkedList<Member> Categories
                { get => categories; }

            #region Get Member
            /// <summary>
            ///     Gets an Instace of a Saved Category 
            /// </summary>
            /// <param name="Category">The Category to search for</param>
            /// <returns>
            ///     member Instance if found else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     Member Found Contains an invalid Rank
            /// </exception>
            public static Member getCategory(byte Category)
            {
                foreach (Member m in Categories)
                {
                    if (m.Value == Category)
                        if (m.MemberRank == Rank.Category)
                        {
                            return m;
                        } else
                        {
                            throw new Exceptions.OPCode.InvalidRankFound(Rank.Category, m.MemberRank);
                        }
                }
                return null;
            }
            
            /// <summary>
            ///     Gets a Subcategory Member depending on a byte according to the byte of Category
            /// </summary>
            /// <param name="Category">The Category to search in</param>
            /// <param name="SubCategory">The Subategory to search for</param>
            /// <returns>
            ///     member Instance if found else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     Member Found Contains an invalid Rank
            /// </exception>
            public static Member getSubCategory(byte Category, byte SubCategory)
            {
                Member Cat;
                if ((Cat = Member.getCategory(Category)) == null)
                    return null;
                foreach (Member m in Cat.Sub)
                {
                    if (m.Value == SubCategory)
                        if (m.MemberRank == Rank.SubCategory)
                        {
                            return m;
                        }
                        else
                        {
                            throw new Exceptions.OPCode.InvalidRankFound(Rank.SubCategory, m.MemberRank);
                        }
                }
                return null;
            }
            /// <summary>
            ///     Gets a Subcategory Member depending on a byte from a Category member
            /// </summary>
            /// <param name="Category">The Category to search in</param>
            /// <param name="SubCategory">The Subategory to search for</param>
            /// <returns>
            ///     member Instance if found else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     Member Found Contains an invalid Rank
            /// </exception>
            public static Member getSubCategory(Member Category, byte SubCategory)
            {
                if (Category.MemberRank != Rank.Category)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Category, Category.MemberRank);
                foreach (Member m in Category.Sub)
                {
                    if (m.Value == SubCategory)
                        if (m.MemberRank == Rank.SubCategory)
                        {
                            return m;
                        }
                        else
                        {
                            throw new Exceptions.OPCode.InvalidRankFound(Rank.SubCategory, m.MemberRank);
                        }
                }
                return null;
            }

            /// <summary>
            ///     Gets a Command Member depending on a byte accoring to the bytes of Category and SubCategory
            /// </summary>
            /// <param name="Category">The Category to search in</param>
            /// <param name="SubCategory">The Subategory to search in</param>
            /// <param name="Command">The Command to search for</param>
            /// <returns>
            ///     member Instance if found else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     Member Found Contains an invalid Rank
            /// </exception>
            public static Member getCommand(byte Category, byte SubCategory, byte Command)
            {
                Member Cat;
                if ((Cat = Member.getSubCategory(Category, SubCategory)) == null)
                    return null;
                foreach (Member m in Cat.Sub)
                {
                    if (m.Value == Command)
                        if (m.MemberRank == Rank.Command)
                        {
                            return m;
                        }
                        else
                        {
                            throw new Exceptions.OPCode.InvalidRankFound(Rank.Command, m.MemberRank);
                        }
                }
                return null;
            }
            /// <summary>
            ///     Gets a Command Member depending on a byte from a Subcategory Member
            /// </summary>
            /// <param name="SubCategory">The Subategory to search in</param>
            /// <param name="Command">The Command to search for</param>
            /// <returns>
            ///     member Instance if found else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     Member Found Contains an invalid Rank
            /// </exception>
            public static Member getCommand(Member SubCategory, byte Command)
            {
                if (SubCategory.MemberRank != Rank.SubCategory)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.SubCategory, SubCategory.MemberRank);
                foreach (Member m in SubCategory.Sub)
                {
                    if (m.Value == Command)
                        if (m.MemberRank == Rank.Command)
                        {
                            return m;
                        }
                        else
                        {
                            throw new Exceptions.OPCode.InvalidRankFound(Rank.Command, m.MemberRank);
                        }
                }
                return null;
            }

            /// <summary>
            ///     Gets a Detail Member depending on a byte accoring to the bytes of Category, SubCategory and Command
            /// </summary>
            /// <param name="Category">The Category to search in</param>
            /// <param name="SubCategory">The Subategory to search in</param>
            /// <param name="Command">The Command to search in</param>
            /// <param name="Detail">The Detail to search for</param>
            /// <returns>
            ///     member Instance if found else the first instance, in otherwords a Detail with value of <see cref="Base.DET_UNDETAILED"/>
            /// </returns>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     Member Found Contains an invalid Rank
            /// </exception>
            public static Member getDetail(byte Category, byte SubCategory, byte Command, byte Detail)
            {
                Member Cat;
                if ((Cat = Member.getCommand(Category, SubCategory, Command)) == null)
                    return null;
                foreach (Member m in Cat.Sub)
                {
                    if (m.Value == Command)
                        if (m.MemberRank == Rank.Detail)
                        {
                            return m;
                        }
                        else
                        {
                            throw new Exceptions.OPCode.InvalidRankFound(Rank.Detail, m.MemberRank);
                        }
                }
                return Cat.Sub.First<Member>();
            }
            /// <summary>
            ///     Gets a Detail Member depending on a byte from a Command Member
            /// </summary>
            /// <param name="Command">The Command to search in</param>
            /// <param name="Detail">The Detail to search for</param>
            /// <returns>
            ///     member Instance if found else the first instance, in otherwords a Detail with value of <see cref="Base.DET_UNDETAILED"/>
            /// </returns>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     Member Found Contains an invalid Rank
            /// </exception>
            public static Member getDetail(Member Command, byte Detail)
            {
                if (Command.MemberRank != Rank.Command)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Command, Command.MemberRank);
                foreach (Member m in Command.Sub)
                {
                    if (m.Value == Detail)
                        if (m.MemberRank == Rank.Detail)
                        {
                            return m;
                        }
                        else
                        {
                            throw new Exceptions.OPCode.InvalidRankFound(Rank.Detail, m.MemberRank);
                        }
                }
                return Command.Sub.First();
            }
            #endregion

            #region Add Member
            /// <summary>
            /// Adds a new Category into the OPCode structure
            /// </summary>
            /// <param name="Category">The byte Defining the Category</param>
            /// <param name="Definition">Definition of the Category</param>
            /// <returns>
            /// <c>true</c> Category has been added<br />
            /// <c>false</c> Category already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            public static bool AddCategory(byte Category, string Definition)
            {
                if(Categories.Count<Member>() == 0 || !Base.isInitialized)
                    throw new Exceptions.ClassUninitialized("OPCode.Base");
                foreach(Member m in Categories)
                {
                    if (m.Value == Category)
                        return false;
                }
                categories.AddLast(new Member(Rank.Category, Category, Definition));
                return true;
            }
            /// <summary>
            /// Adds a Category into the OPCode structure
            /// </summary>
            /// <param name="Category">The Member Category you want to add</param>
            /// <returns>
            /// <c>true</c> Member has been added<br />
            /// <c>false</c> Member already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     A Member contains an innapropriate Rank
            /// </exception>
            public static bool AddCategory(Member Category)
            {
                if (Category.value == 0x00 && !Base.isInitialized)
                    throw new Exceptions.ClassUninitialized("OPCode.Base");
                if(Category.MemberRank != Rank.Category)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Category, Category.MemberRank);
                foreach (Member m in Categories)
                {
                    if (m.Value == Category.value)
                        return false;
                }
                categories.AddLast(Category);
                return true;
            }

            /// <summary>
            ///     Adds a new Subcategory into the OPCode Structure
            /// </summary>
            /// <param name="Category">The Parent Category</param>
            /// <param name="SubCategory">The byte difining the SubCategory</param>
            /// <param name="Definition">The Difinition fo the Subcategory</param>
            /// <returns>
            /// <c>true</c> Member has been added<br />
            /// <c>false</c> Member already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     A Member contains an innapropriate Rank
            /// </exception>
            public static bool AddSubCategory(Member Category, byte SubCategory, string Definition)
            {
                if(Categories.Count<Member>() == 0)
                    throw new Exceptions.ClassUninitialized("OPCode.Base");
                if(Category.MemberRank != Rank.Category)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Category, Category.MemberRank);
                foreach (Member m in Category.Sub)
                {
                    if (m.Value == SubCategory)
                        return false;
                }
                Member sub = new Member(Rank.SubCategory, SubCategory, Definition);
                Category.sub.AddLast(sub);
                return true;
            }
            /// <summary>
            ///     Adds a Subcategory into the OPCode Structure
            /// </summary>
            /// <param name="Category">The Parent Category</param>
            /// <param name="SubCategory">The SubCategory Member to add</param>
            /// <returns>
            /// <c>true</c> Member has been added<br />
            /// <c>false</c> Member already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     A Member contains an innapropriate Rank
            /// </exception>
            public static bool AddSubCategory(Member Category, Member SubCategory)
            {
                if(Category.MemberRank != Rank.Category)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Category, Category.MemberRank);
                if(SubCategory.MemberRank != Rank.SubCategory)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.SubCategory, SubCategory.MemberRank);
                foreach (Member m in Category.Sub)
                {
                    if (m.Value == SubCategory.value)
                        return false;
                }
                Category.sub.AddLast(SubCategory);
                return true;
            }

            /// <summary>
            ///     Adds a new Command into the OPCode structure
            /// </summary>
            /// <param name="SubCategory">Parent SubCategory</param>
            /// <param name="Command">The byte defining the Command</param>
            /// <param name="Definition">The definition of the Command</param>
            /// <returns>
            /// <c>true</c> Member has been added<br />
            /// <c>false</c> Member already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     A Member contains an innapropriate Rank
            /// </exception>
            public static bool AddCommand(Member SubCategory, byte Command, string Definition)
            {
                if(Categories.Count<Member>() == 0)
                    throw new Exceptions.ClassUninitialized("OPCode.Base");
                if (SubCategory.MemberRank != Rank.SubCategory)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.SubCategory, SubCategory.MemberRank);
                foreach (Member m in SubCategory.Sub)
                {
                    if (m.Value == Command)
                        return false;
                }
                Member cmd = new Member(Rank.Command, Command, Definition);
                SubCategory.sub.AddLast(cmd);
                return true;
            }
            /// <summary>
            ///     Adds a Command into the OPCode structure
            /// </summary>
            /// <param name="SubCategory">Parent SubCategory</param>
            /// <param name="Command">The Command Member to add</param>
            /// <returns>
            /// <c>true</c> Member has been added<br />
            /// <c>false</c> Member already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     A Member contains an innapropriate Rank
            /// </exception>
            public static bool AddCommand(Member SubCategory, Member Command)
            {
                if (SubCategory.MemberRank != Rank.SubCategory)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.SubCategory, SubCategory.MemberRank);
                if (Command.MemberRank != Rank.Command)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Command, Command.MemberRank);
                foreach (Member m in SubCategory.Sub)
                {
                    if (m.Value == Command.value)
                        return false;
                }
                SubCategory.sub.AddLast(Command);
                return true;
            }

            /// <summary>
            ///     Adds a new Detail into the OPCode structure
            /// </summary>
            /// <param name="Command">Parent Command</param>
            /// <param name="Detail">The byte defining the Detail</param>
            /// <param name="Definition">The definition of the Detail</param>
            /// <returns>
            /// <c>true</c> Member has been added<br />
            /// <c>false</c> Member already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     A Member contains an innapropriate Rank
            /// </exception>
            public static bool AddDetail(Member Command, byte Detail, string Definition)
            {
                
                if(Categories.Count<Member>() == 0)
                    throw new Exceptions.ClassUninitialized("OPCode.Base");
                if (Command.MemberRank != Rank.Command)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Command, Command.MemberRank);
                foreach (Member m in Command.Sub)
                {
                    if (m.Value == Detail)
                        return false;
                }
                categories.AddLast(new Member(Rank.Detail, Detail, Definition));
                return true;
            }
            /// <summary>
            ///     Adds a Detail into the OPCode structure
            /// </summary>
            /// <param name="Command">Parent Command</param>
            /// <param name="Detail">The Detail to add</param>
            /// <returns>
            /// <c>true</c> Member has been added<br />
            /// <c>false</c> Member already exists
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     OPCode.Base was not intialized
            /// </exception>
            /// <exception cref="Exceptions.OPCode.InvalidRankFound">
            ///     A Member contains an innapropriate Rank
            /// </exception>
            public static bool AddDetail(Member Command, Member Detail)
            {
                if (Command.MemberRank != Rank.Command)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Command, Command.MemberRank);
                if (Detail.MemberRank != Rank.Detail)
                    throw new Exceptions.OPCode.InvalidRankFound(Rank.Detail, Detail.MemberRank);
                foreach (Member m in Command.Sub)
                {
                    if (m.Value == Detail.value)
                        return false;
                }
                Command.sub.AddLast(Detail);
                return true;
            }
            #endregion

            #endregion

            private Rank rank;
            private byte value;
            private string definition = "Undefined Member";
            private LinkedList<Member> sub = new LinkedList<Member>();

            /// <summary>
            ///     Returns the Member's Rank
            /// </summary>
            public Rank MemberRank
                { get => rank; }
            /// <summary>
            ///     Returns The Member's Byte Value
            /// </summary>
            public byte Value
                { get => value; }
            /// <summary>
            ///     Returns The Member's Definition
            /// </summary>
            public string Definition
                { get => definition; }
            /// <summary>
            ///     Returns The Member's Sub Members 
            /// </summary>
            /// <remarks>
            ///    > [!NOTE]
            ///    > Detail Members Should not have sub members
            /// </remarks>
            public LinkedList<Member> Sub
                { get => sub; }

            internal Member() {}

            /// <summary>
            ///     Creates a new Member instance 
            /// </summary>
            /// <remarks>
            ///     Use Appropriate Functions to add this into the OPCode structure or else it would be rendred useless<br/><br/> See Also <br/>
            ///     <seealso cref="AddCategory(Member)"/><br/>
            ///     <seealso cref="AddSubCategory(Member, Member)"/><br/>
            ///     <seealso cref="AddCommand(Member, Member)"/><br/>
            ///     <seealso cref="AddDetail(Member, Member)"/>
            /// </remarks>
            /// 
            /// <param name="Rank"></param>
            /// <param name="Value"></param>
            /// <param name="Definition"></param>
            public Member(Rank Rank, byte Value, string Definition)
            {
                rank = Rank;
                value = Value;
                definition = Definition;
                if(Rank == Rank.Command)
                    sub.AddLast(Undefined_Detail.clone());
            }

            /// <summary>
            ///     Clones the Member
            /// </summary>
            /// <returns>
            ///     A Clone of the Member instance
            /// </returns>
            public Member clone()
                => new Member(rank, value, definition);
        }
        /// <summary>
        /// This Class Defined THe Base OPCode Structure for Basic SNF Functioning
        /// </summary>
        public class Base
        {
            #region [Base OPCodes Byte Definitions]
            /// <summary>
            ///     Base Category for SNF
            /// </summary>
            public static byte CATEGORY         => 0x00;
            /// <summary>
            ///     Base Sub-Category for SNF
            /// </summary>
            public static byte SUBCATEGORY      => 0x00;
            /// <summary>
            ///     Default Detail for every Command
            /// </summary>
            public static byte DET_UNDETAILED   => 0x00;


            /// <summary>
            ///     When client attemps to connect.
            /// </summary>
            public static byte CMD_CONNECT      => 0x00;
            /// <summary>
            ///     When client attemps to reconnect - or forced to.
            /// </summary>
            public static byte CMD_RECONNECT    => 0x01;
            /// <summary>
            ///     When client attemps to disconnect.
            /// </summary>
            public static byte CMD_DISCONNECT   => 0x02;
            /// <summary>
            ///     When client requests SNF version of the Server.
            /// </summary>
            public static byte CMD_SNF_VER      => 0x03;
            /// <summary>
            ///     When client was forced to disconnect ( Kicked ).
            /// </summary>
            public static byte CMD_KICK         => 0x04;
            /// <summary>
            ///     When client's reuqest was confirmed.
            /// </summary>
            public static byte CMD_CONFIRM      => 0x05;
            /// <summary>
            ///     When client's request was rejected.
            /// </summary>
            public static byte CMD_REJECT       => 0x06;

            #region Invalid Command
            /// <summary>
            ///     When client's request was invalid either, wrong version or incomplete request.
            /// </summary>
            public static byte CMD_INVALID      => 0xFF;
            /// <summary>
            ///     Received opcode on the server side was not registred.
            /// </summary>
            public static byte DET_INVALID_UNREGISTRED_OPCODE   => 0x00;
            /// <summary>
            ///     Protocol used is invalid.
            /// </summary>
            public static byte DET_INVALID_ERROR_PROTOCOL       => 0x01;
            /// <summary>
            ///     Sent OPCode does not have a function to call on the server side.
            /// </summary>
            public static byte DET_INVALID_UNIMPLEMENTED_OPCODE => 0x02;
            #endregion
            #endregion

            private static bool isInit = false;
            /// <summary>
            ///     Returns if OPCode.Base is initialized or not
            /// </summary>
            public static bool isInitialized 
                => isInit;

            private static Member MEM_CATEGORY      = null;
            private static Member MEM_SUBCATEGORY   = null;
            private static Member MEM_INVALID       = null;

            /// <summary>
            ///     Initializes OPCode.Base
            /// </summary>
            public static void Initialize()
            {
                isInit = true;
                MEM_CATEGORY = new Member(Member.Rank.Category, CATEGORY, "Base SNF OPCode Category");
                Member.AddCategory(MEM_CATEGORY);

                MEM_SUBCATEGORY = new Member(Member.Rank.SubCategory, SUBCATEGORY, "Base SNF OPCode SUbCategory");
                Member.AddSubCategory(MEM_CATEGORY, MEM_SUBCATEGORY);

                Member.AddCommand(MEM_SUBCATEGORY, CMD_CONNECT,     "Used when client is attempting to connect");
                Member.AddCommand(MEM_SUBCATEGORY, CMD_RECONNECT,   "Used when client is attempting to reconnect or is forced to.");
                Member.AddCommand(MEM_SUBCATEGORY, CMD_DISCONNECT,  "When client attemps to disconnect.");
                Member.AddCommand(MEM_SUBCATEGORY, CMD_SNF_VER,     "When client requests SNF version of the Server.");
                Member.AddCommand(MEM_SUBCATEGORY, CMD_KICK,        "When client was forced to disconnect ( Kicked ).");
                Member.AddCommand(MEM_SUBCATEGORY, CMD_CONFIRM,     "When client's reuqest was confirmed.");
                Member.AddCommand(MEM_SUBCATEGORY, CMD_REJECT,      "When client's request was rejected.");

                MEM_INVALID = new Member(
                    Member.Rank.Command, 
                    CMD_INVALID, 
                    "When client's request was invalid either, wrong version or incomplete request."
                    );

                Member.AddCommand(MEM_SUBCATEGORY, MEM_INVALID);

                Member.AddDetail(MEM_INVALID, DET_INVALID_UNREGISTRED_OPCODE,   "Received opcode was not registred.");
                Member.AddDetail(MEM_INVALID, DET_INVALID_ERROR_PROTOCOL,       "Protocol used is invalid.");
                Member.AddDetail(MEM_INVALID, DET_INVALID_UNIMPLEMENTED_OPCODE, "Sent OPCode does not have a function to call on the server side.");
            }
            /// <summary>
            ///     Returns the Base Category
            /// </summary>
            /// <returns><c>null</c> if Class is uninitialized</returns>
            public static Member getCategory()
                => MEM_CATEGORY;
            /// <summary>
            ///     Returns the base Subcategory
            /// </summary>
            /// <returns></returns>
            public static Member getSubcategory()
                => MEM_CATEGORY;
            /// <summary>
            ///     Returns the base Command
            /// </summary>
            /// <param name="Command">base command to search for</param>
            /// <returns><c>null</c> if Class is uninitialized or not found</returns>
            public static Member getCommand(byte Command)
            {
                if(MEM_SUBCATEGORY == null)
                    return null;
                foreach(Member m in MEM_SUBCATEGORY.Sub)
                {
                    if(m.Value == Command)
                        return m;
                }
                return null;
            }
            /// <summary>
            ///     Returns the base Command with a certain Detail
            /// </summary>
            /// <param name="Command">Base Command</param>
            /// <param name="Detail">Base Command's Detail</param>
            /// <returns><c>null</c> if Class is uninitialized or command not found
            /// and the Undetailed 'Detail' if Detail wasnot found
            /// </returns>
            public static Member getDetail(byte Command, byte Detail)
            {
                Member A = getCommand(Command);
                if(A == null)
                    return null;

                foreach (Member m in A.Sub)
                {
                    if (m.Value == Command)
                        return m;
                }
                return A.Sub.First();
            }
            /// <summary>
            ///     Gets an OPCode using a Base Command without any detail
            /// </summary>
            /// <remarks>
            ///     To Specify a Detail :<br/> <seealso cref="get(byte, byte)"/>
            /// </remarks>
            /// <param name="Command">Base Command</param>
            /// <returns>
            ///     <c>OPCode Instance</c> if Command is a valid Base Command  else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     Thrown When The OPCode.Base Class was not Initialized 
            /// </exception>
            /// <exception cref="Exceptions.OPCode.Unregistred">
            ///     <paramref name="Command"/> Is not a registred OPCode Command 
            /// </exception>
            public static OPCode get(byte Command) 
                => get(Command, DET_UNDETAILED);
            /// <summary>
            ///     Gets an OPCode using a Base Command with a detail
            /// </summary>
            /// <param name="Command">Base Command</param>
            /// <param name="Detail">Base Command's Detail</param>
            /// <returns>
            ///     <c>OPCode Instance</c> if Command is a valid Base Command  else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     Thrown When The OPCode.Base Class was not Initialized 
            /// </exception>
            /// <exception cref="Exceptions.OPCode.Unregistred">
            ///     <paramref name="Command"/> Is not a registred OPCode Command 
            /// </exception>
            public static OPCode get(byte Command, byte Detail)
            {
                Member cmd;
                Member det;

                if ((cmd = Member.getCommand(MEM_SUBCATEGORY, Command)) == null)
                    throw new Exceptions.OPCode.Unregistred(Command);

                det = Member.getDetail(cmd, Command);
                return OPCode.get(MEM_CATEGORY, MEM_SUBCATEGORY, cmd, det);
            }
            /// <summary>
            ///     Gets an OPCode using a 'Invalid' Command without any detail
            /// </summary>
            /// <remarks>
            ///     To Specify a Detail :<br/> <seealso cref="getInvalid(byte)"/>
            /// </remarks>
            /// <returns>
            ///     <c>OPCode Instance</c> if Command is a valid Base Command  else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     Thrown When The OPCode.Base Class was not Initialized 
            /// </exception>
            public static OPCode getInvalid() 
                => getInvalid(DET_UNDETAILED);
            /// <summary>
            ///     Gets an OPCode using a 'Invalid' Command with a detail
            /// </summary>
            /// <returns>
            ///     <c>OPCode Instance</c> if Command is a valid Base Command  else <c>null</c>
            /// </returns>
            /// <exception cref="Exceptions.ClassUninitialized">
            ///     Thrown When The OPCode.Base Class was not Initialized 
            /// </exception>
            public static OPCode getInvalid(byte Detail) =>
                OPCode.get(
                    MEM_CATEGORY, 
                    MEM_SUBCATEGORY, 
                    MEM_INVALID, 
                    Member.getDetail(MEM_INVALID, Detail)
                    ); 
        }
    }
}
