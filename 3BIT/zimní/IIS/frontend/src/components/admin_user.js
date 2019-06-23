import React, {Component} from 'react';
import {Link, Redirect} from "react-router-dom";
import axios from 'axios';
//
import {API_BASE, ROUTE_PREFIX, getUser, encodeEmailHash, decodeEmailHash, on403} from '../const.js'
// komponenty
import withFetching from './hoc_fetching.js'

class AdminUsersPage extends Component {
    constructor(props) {
        super(props)
        this.state = {
            redirectOk: false,
            redirectPassword: false,
            selectedUser: ''
        }
    }

    buttonHandler = (email, type, event) => {
        if (type === 'user') {
            this.setState({
                selectedUser: encodeEmailHash(email),
                redirectOk: true,
            })
        } else {
            this.setState({
                selectedUser: encodeEmailHash(email),
                redirectPassword: true
            })
        }
    }

    deleteUser = async(email, type, auth, event) => {
        if (getUser()['email'] === email) {
            alert('Nelze upravit současného uživatele')
        } else {
            if (window.confirm('Vážně chcete smazat uživatele?')) {
                try {
                    var response = await axios({
                        method: 'delete',
                        url: `${API_BASE}user/`,
                        headers: {
                            token: getUser()['token']
                        },
                        data: {
                            email: email
                        }
                    })
                    this.setState({ok: true})
                } catch (error) {
                    alert(`Nepodařilo se odstranit uživatele`)
                }

                if (auth < 4) {
                    try {
                        var response = await axios({
                            method: 'delete',
                            url: `${API_BASE}customer/`,
                            headers: {
                                token: getUser()['token']
                            },
                            data: {
                                email: email
                            }
                        })
                        this.setState({ok: true})
                    } catch (error) {
                        alert(`Nepodařilo se odstranit uživatele`)
                    }
                }
                window.location.reload()
            }
        }
    }

    render() {
        var users = this.props.data ? this.props.data : []

        if (this.state.redirectOk) {
            return <Redirect to={`${ROUTE_PREFIX}admin/uzivatele/detail/${this.state.selectedUser}`}></Redirect>
        }

        if (this.state.redirectPassword) {
            return <Redirect to={`${ROUTE_PREFIX}admin/uzivatele/heslo/${this.state.selectedUser}`}></Redirect>
        }

        return (
            <div>
                <nav className="subHeader__nav--small">
                    <div className="layout__inner">
                        <Link to={`${ROUTE_PREFIX}admin`}>Administrace</Link>
                    </div>
                </nav>
                <div className="admin layout__inner">
                    <div className="admin__start__text">
                         <h1>Správa uživatelů</h1>
                        <small>Co by jste si přáli dělat?</small>
                    </div>

                    <div className="admin__buttons">
                        <Link className="button" to={`${ROUTE_PREFIX}admin/uzivatele/pridat/0`}>Přidat uživatele</Link>
                        <Link className="button" to={`${ROUTE_PREFIX}admin/uzivatele/pridat/1`}>Přidat maloobchodníka</Link>
                        <Link className="button" to={`${ROUTE_PREFIX}admin/uzivatele/pridat/2`}>Přidat velkoobchodníka</Link>
                    </div>

                    
                    <table className="admin__table" cellSpacing="0" cellPadding="0">
                        <thead>
                            <tr>
                                <th>Jméno</th>
                                <th>Email</th>
                                <th>Role</th>
                                <th>Akce</th>
                            </tr>
                        </thead>
                        <tbody>
                            {users.map((user) => {
                                var name = ''
                                if (user.opravneni === 5) {
                                    name = 'administrátor'
                                } else if (user.opravneni === 4) {
                                    name = 'produktový manažer'
                                } else if (user.opravneni === 2) {
                                    name = 'velkoobchodník'
                                } else if (user.opravneni === 1) {
                                    name = 'maloobchodník'
                                }

                                var editUser = (
                                    <button className="button__secondary" onClick={(event) => this.buttonHandler(user.email, 'user', event)}>Upravit uživatele</button>
                                )

                                if (user.opravneni > 3) {
                                    editUser = null
                                }

                                return (
                                    <tr key={user.email}>
                                        <td>{user.jmeno}</td>
                                        <td>{user.email}</td>
                                        <td>{name}</td>
                                        <td>
                                            {editUser}
                                            <button className="button__secondary" onClick={(event) => this.buttonHandler(user.email, 'password', event)}>Upravit heslo</button>
                                            <button className="button__secondary" onClick={(event) => this.deleteUser(user.email, 'delete', user.opravneni, event)}>Smazat</button>
                                        </td>
                                    </tr>
                                ) 
                            })}
                        </tbody>
                    </table>
                </div>
            </div>
        )
    }
}

export const AdminUsers = withFetching(`${API_BASE}user/`)(AdminUsersPage)

export class AdminUsersDetail extends Component {
    constructor(props) {
        super(props)
        
        this.state = {
            email               : '',
            name                : '',
            password            : '',
            passwordCheck       : '',
            authority           : '4',
            address             : 'sample',
            passwordMessage     : 'zadejte heslo a jeho ověření',
            userId              : '',
            ok                  : false,

            working             : '',
            phone: '',
            personalPhone: '',
            callee: '',
            zip: '',
            ic: '',
            dic: '',
            route: '',
            city: ''
        }
    }

    componentDidMount() {
        if (this.props.match.url.indexOf('heslo') === -1 && this.props.match.url.indexOf('detail') !== -1) {
            this.setState({
                userId: decodeEmailHash(this.props.match.params.userId)
            })

            this.fetchUserToUpdate()
        }
        
    }

    async fetchUserToUpdate() {
        try {
            // najdeme data na upravu
            var response = await axios({
                method: 'get',
                url: `${API_BASE}customer/`,
                headers: {
                    token: getUser()['token']
                }
            })

            response.data.forEach((cur, idx) => {
                if (cur.email === this.state.userId) {
                    //console.log(cur)

                    var jmeno = cur.jmeno_firmy
                    if (!jmeno) {
                        jmeno = cur.jmeno
                    }


                    this.setState({
                        name: jmeno,
                        email: cur.email,
                        authority: cur.level,
                        route: cur.ulice,
                        zip: cur.psc,
                        city: cur.mesto,
                        phone: cur.telefon
                    })
                }
            })
        } catch (error) {
            on403()
        }
    }

    handleChange = (type, event) => {
        if (type.indexOf('password') !== -1) {
            var a = ''
            var b = ''
            if (type === 'password') {
                a = event.target.value
                b = this.state.passwordCheck
            } else if (type === 'passwordCheck') {
                a = event.target.value
                b = this.state.password
            }
            // validace hesel
            if (a !== b) {
                this.setState({passwordMessage: 'hesla se nerovnají'})
            } else {
                if (a.length < 6) {
                    this.setState({passwordMessage: 'příliš krátké'})
                } else {
                    this.setState({passwordMessage: 'v pořádku'})
                }
            }
        }

        let obj = {}
        obj[type] = event.target.value
        this.setState(obj)
    }

    resetForm() {
        this.setState({
            email               : '',
            name                : '',
            password            : '',
            passwordCheck       : '',
            authority           : '4',
            address             : '',
            passwordMessage     : 'zadejte heslo a jeho ověření',
            phone: '',
            personalPhone: '',
            callee: '',
            zip: '',
            ic: '',
            dic: '',
            route: '',
            city: ''
        })
    }


    async postUser(obj) {
        console.log(obj)
        try {
            var response = await axios({
                method: 'post',
                url: `${API_BASE}user/`,
                headers: {
                    token: getUser()['token']
                },
                data: obj
            })
            this.setState({ok: true})
        } catch (error) {
            this.resetForm()
            if (error.response.status === 400) {
                alert(`Nepodařilo se vytvořit uživatele - ${error.response.data}`)
            } else {
                on403()
            }
        }
    }

    async postUserCustomer(obj, type, method) {
        var objUser = {
            jmeno       : obj.jmeno,
            email       : obj.email,
            heslo       : obj.heslo,
            opravneni   : type
        }
        var objCustomer = {
            telefon: obj.telefon,
            email: obj.email,
            ulice: obj.ulice,
            mesto: obj.mesto,
            psc: obj.psc
        }

        if (type === 1) {
            objCustomer['jmeno'] = obj.jmeno
            objCustomer['prijmeni'] = ''
            objCustomer['osobni_telefon'] = obj.osobni_telefon ? obj.osobni_telefon : '000000000'
            objCustomer['osloveni'] = obj.osloveni
        } else if (type === 2) {
            objCustomer['jmeno_firmy'] = obj.jmeno
            objCustomer['dic'] = obj.dic
            objCustomer['ic'] = obj.ic
            objCustomer['osloveni'] = ''
        }


        //nejdrive pridame uzivatele
        try {
            var response = await axios({
                method: method,
                url: `${API_BASE}user/`,
                headers: {
                    token: getUser()['token']
                },
                data: objUser
            })
            //this.setState({ok: true})
        } catch (error) {
            this.resetForm()
            if (error.response.status === 400) {
                alert(`Nepodařilo se vytvořit uživatele - ${error.response.data}`)
            } else {
                on403()
            }
        }

        try {
            var response = await axios({
                method: method,
                url: `${API_BASE}customer/`,
                headers: {
                    token: getUser()['token']
                },
                data: objCustomer
            })
            this.setState({ok: true})
        } catch (error) {
            this.resetForm()
            if (error.response.status === 400) {
                alert(`Nepodařilo se vytvořit uživatele`)
            } else {
                on403()
            }
        }
    }

    handleSubmit = async (type, event) => {   
        event.preventDefault();

        var addMatch = parseInt(this.props.match.params.addId, 10)
        var userMatch = this.props.match.params.userId

        // nesedí hesla
        if (this.state.password !== this.state.passwordCheck) {
            alert('Nesedí kontrola hesla')
            this.setState({
                password: '',
                passwordCheck: '',
                passwordMessage     : 'zadejte heslo a jeho ověření'
            })
        } else {
            // pro update hesla
            if (type === 'password') {
                try {
                    var response = await axios({
                        method: 'put',
                        url: `${API_BASE}user/`,
                        headers: {
                            token: getUser()['token']
                        },
                        data: {
                            email: decodeEmailHash(this.props.match.params.userId),
                            heslo: this.state.password
                        }
                    })
                    this.setState({ok: true})
                } catch (error) {
                    this.resetForm()
                    if (error.response.status === 403) {
                        on403()
                    } else {
                        alert(`Nepodařilo se změnit heslo`)
                    }
                }
            } 
            // pridani uzivatelu
            else if (type === 'add') {
                var obj = {
                    jmeno: this.state.name,
                    email: this.state.email,
                    heslo: this.state.password,
                }
                //
                if (!addMatch) {
                    obj['opravneni'] = parseInt(this.state.authority, 10)
                    this.postUser(obj)
                } else {
                    obj['ulice'] = this.state.route
                    obj['mesto'] = this.state.city
                    obj['psc'] = this.state.zip
                    obj['telefon'] = this.state.phone

                    if (addMatch === 1) {
                        obj['opravneni'] = 1
                        obj['osobni_telefon'] =  this.state.personalPhone
                        obj['osloveni'] =  this.state.callee
                        //
                        this.postUserCustomer(obj, 1, 'post')
                    } else if (addMatch === 2) {
                        obj['opravneni'] = 2
                        obj['ic'] = this.state.ic
                        obj['dic'] = this.state.dic
                        //
                        this.postUserCustomer(obj, 2, 'post')
                    }
                }
            } 
            // update uzivatelu
            else if (type === 'update') {
                var obj = {
                    jmeno: this.state.name,
                    email: this.state.email,
                    heslo: this.state.password,
                }

                obj['ulice'] = this.state.route
                obj['mesto'] = this.state.city
                obj['psc'] = this.state.zip
                obj['telefon'] = this.state.phone


                this.postUserCustomer(obj, 1, 'put')
            }
        }
    }


    render() {
        if (this.state.ok) {
            return <Redirect to={`${ROUTE_PREFIX}admin/uzivatele`}></Redirect>
        }

        var navBar = (
            <nav className="subHeader__nav--small">
                <div className="layout__inner">
                    <Link to={`${ROUTE_PREFIX}admin`}>Administrace</Link>
                    <span className="subHeader__separator">/</span>
                    <Link to={`${ROUTE_PREFIX}admin/uzivatele`}>Uživatelé</Link>
                </div>
            </nav>
        )

        var addMatch = this.props.match.params.addId
        var userMatch = this.props.match.params.userId
        var isPasswordPage = this.props.match.url.indexOf('heslo') !== -1 ? true : false

        var passwd = (
            <span>
                <br/>
                <label>
                        <span>Heslo</span>
                        <input 
                            type="password" 
                            required 
                            value={this.state.password} onChange={(event) => this.handleChange('password', event)} 
                        />
                    </label>
                    <label>
                        <span>Kontrola hesla</span>
                        <input 
                            type="password" 
                            required 
                            value={this.state.passwordCheck} onChange={(event) => this.handleChange('passwordCheck', event)} 
                        />
                    </label>
                    <small className="admin__passwordCheck">{this.state.passwordMessage}</small>
            </span>
        )

        var place = (
            <span>
                <br/>
                <label>
                    <span>Ulice</span>
                    <input 
                        type="text" 
                        required 
                        maxLength="32"
                        value={this.state.route} 
                        onChange={(event) => this.handleChange('route', event)} 
                    />
                </label>
                <label>
                    <span>Město</span>
                    <input 
                        type="text" 
                        required 
                        maxLength="32"
                        value={this.state.city} 
                        onChange={(event) => this.handleChange('city', event)} 
                    />
                </label>
                    <label>
                    <span>PSČ</span>
                    <input 
                        required 
                        type='tel'
                        pattern=".{5,}"  
                        minLength="5"
                        maxLength="5"
                        value={this.state.zip} 
                        onChange={(event) => this.handleChange('zip', event)} 
                    />
                </label>
            </span>
        )

        if (addMatch) {
            var userType = ''
            var formCtx
            // pro INT
            addMatch = parseInt(addMatch, 10)
            //
            if (!addMatch) {
                userType = 'uživatel'
                formCtx = (
                    <span>
                        <label>
                            <span>Jméno</span>
                            <input 
                                type="text" 
                                required 
                                maxLength="64"
                                value={this.state.name} 
                                onChange={(event) => this.handleChange('name', event)} 
                            />
                        </label>
                        <label>
                            <span>E-mail</span>
                            <input 
                                type="email" 
                                required 
                                maxLength="64"
                                value={this.state.email} 
                                onChange={(event) => this.handleChange('email', event)} 
                            />
                        </label>
                        <label>
                            <span>Oprávnění</span>
                                <select onChange={(event) => this.handleChange('authority', event)}>
                                    <option key="4" value="4">produktový manažer</option>
                                    <option key="5" value="5">administrátor</option>
                                </select>
                        </label>
                        {passwd}
                    </span>
                )

            } else if (addMatch === 1) {
                userType = 'maloobchodník'
                formCtx = (
                    <span>
                        <label>
                            <span>Jméno</span>
                            <input 
                                type="text" 
                                required 
                                maxLength="64"
                                value={this.state.name} 
                                onChange={(event) => this.handleChange('name', event)} 
                            />
                        </label>
                        <label>
                            <span>E-mail</span>
                            <input 
                                type="email" 
                                required 
                                maxLength="64"
                                value={this.state.email} 
                                onChange={(event) => this.handleChange('email', event)} 
                            />
                        </label>
                        <label>
                            <span>Oslovení</span>
                            <input 
                                type="text" 
                                maxLength="5"
                                value={this.state.callee} 
                                onChange={(event) => this.handleChange('callee', event)} 
                            />
                        </label>
                        <label>
                            <span>Telefon (bez mezer)</span>
                            <input 
                                type='tel'
                                required 
                                pattern=".{9,}"  
                                minLength="9"
                                maxLength="9"
                                value={this.state.phone} 
                                onChange={(event) => this.handleChange('phone', event)} 
                            />
                        </label>
                        <label>
                            <span>Osobní Telefon (bez mezer)</span>
                            <input 
                                type='tel'
                                pattern=".{9,}"  
                                minLength="9"
                                maxLength="9"
                                value={this.state.personalPhone} 
                                onChange={(event) => this.handleChange('personalPhone', event)} 
                            />
                        </label>
                        {place}
                        {passwd}
                    </span>
                )
            } else if (addMatch === 2) {
                userType = 'velkoobchodník'
                formCtx = (
                    <span>
                        <label>
                            <span>Jméno firmy</span>
                            <input 
                                type="text" 
                                required 
                                maxLength="64"
                                value={this.state.name} 
                                onChange={(event) => this.handleChange('name', event)} 
                            />
                        </label>
                        <label>
                            <span>E-mail</span>
                            <input 
                                type="email" 
                                required 
                                maxLength="64"
                                value={this.state.email} 
                                onChange={(event) => this.handleChange('email', event)} 
                            />
                        </label>
                        <label>
                            <span>Telefon (bez mezer)</span>
                            <input 
                                type='tel'
                                required 
                                pattern=".{9,}"  
                                minLength="9"
                                maxLength="9"
                                value={this.state.phone} 
                                onChange={(event) => this.handleChange('phone', event)} 
                            />
                        </label>
                        <label>
                            <span>IČ</span>
                            <input 
                                type="number" 
                                required 
                                maxLength="16"
                                value={this.state.ic} 
                                onChange={(event) => this.handleChange('ic', event)} 
                            />
                        </label>
                        <label>
                            <span>DIČ</span>
                            <input 
                                type="text" 
                                required 
                                maxLength="24"
                                pattern="[C][Z][0-9]{8,10}"
                                value={this.state.dic} 
                                onChange={(event) => this.handleChange('dic', event)} 
                            />
                        </label>
                        {place}
                        {passwd}
                    </span>
                )
            }


            return (
                <div>
                    {navBar}
                    <div className="admin admin__start layout__inner">
                        <div className="admin__start__text">
                            <h1>Přidat uživatele ({userType})</h1>
                        </div>

                        <form onSubmit={(event) => this.handleSubmit('add', event)}>
                            {formCtx}
                            <input type="submit" value="Vytvořit účet" className="button"/>
                            <Link to={`${ROUTE_PREFIX}admin/uzivatele`} className="button__secondary">Zrušit akci</Link>
                        </form>
                    </div>
                </div>
            )

        } else if (userMatch) {
            // upravujeme heslo
            if (isPasswordPage) {
                return (
                    <div>
                        {navBar}
                        <div className="admin admin__start layout__inner">
                            <div className="admin__start__text">
                                <h1>Změnit heslo pro uživatele ({decodeEmailHash(userMatch)})</h1>
                            </div>
                            <form onSubmit={(event) => this.handleSubmit('password', event)}>
                                {passwd}
                                <input type="submit" value="Změnit heslo" className="button"/>
                                <Link to={`${ROUTE_PREFIX}admin/uzivatele`} className="button__secondary">Zrušit akci</Link>
                            </form>
                        </div>
                    </div>
                )
            } else {
                return (
                    <div>
                        {navBar}
                        <div className="admin admin__start layout__inner">
                            <div className="admin__start__text">
                                <h1>Upravit uživatele ({decodeEmailHash(userMatch)})</h1>
                            </div>
                            <form onSubmit={(event) => this.handleSubmit('update', event)}>
                                <label>
                                    <span>Jméno firmy</span>
                                    <input 
                                        type="text" 
                                        required 
                                        maxLength="64"
                                        value={this.state.name} 
                                        onChange={(event) => this.handleChange('name', event)} 
                                    />
                                </label>
                               <label>
                                    <span>Telefon (bez mezer)</span>
                                    <input 
                                        type="number" 
                                        required 
                                        maxLength="9"
                                        value={this.state.phone} 
                                        onChange={(event) => this.handleChange('phone', event)} 
                                    />
                                </label>
                                {place}
                                <input type="submit" value="Upravit účet" className="button"/>
                                <Link to={`${ROUTE_PREFIX}admin/uzivatele`} className="button__secondary">Zrušit akci</Link>
                            </form>
                        </div>
                    </div>
                )
            }
        }
        return navBar
    }
}